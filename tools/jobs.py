#!/usr/bin/env python3
"""Shared parallelism default for ninja, sweeps, searches and Ghidra.

Parallel child worktrees each defaulted to every core (sweep -j 16, uncapped
ninja, Ghidra at half the cores), so a dozen children drove a 16-core host to a
load of 30+ and every build and search slowed down. Every tool now takes its
default from `default_jobs()`:

1. `LSW1_JOBS`, when set to a positive integer, wins.
2. Otherwise the cores are split across the worktrees that ran ninja in the
   last `ACTIVE_WINDOW` seconds (this checkout counts as active), floor 2.

Explicit `-j`/`--jobs`/`--max-cpu` flags still override the default.

    python3 tools/jobs.py            # print the default, e.g. for ninja -j
"""
from __future__ import annotations

import os
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
ACTIVE_WINDOW = 30 * 60
FLOOR = 2


def _worktrees(root: Path) -> list[Path]:
    try:
        out = subprocess.run(["git", "worktree", "list", "--porcelain"], cwd=root,
                             capture_output=True, text=True, timeout=10).stdout
    except (OSError, subprocess.SubprocessError):
        return [root]
    trees = [Path(line[len("worktree "):]) for line in out.splitlines() if line.startswith("worktree ")]
    return trees or [root]


def active_worktrees(root: Path = ROOT, now: float | None = None, window: float = ACTIVE_WINDOW) -> int:
    """Worktrees whose ninja log changed within `window` seconds; at least 1 (this one)."""
    now = time.time() if now is None else now
    here = root.resolve()
    count = 1
    for tree in _worktrees(root):
        if tree.resolve() == here:
            continue
        try:
            if now - (tree / ".ninja_log").stat().st_mtime <= window:
                count += 1
        except OSError:
            pass
    return count


def default_jobs(root: Path = ROOT, cpus: int | None = None) -> int:
    env = os.environ.get("LSW1_JOBS", "").strip()
    if env:
        try:
            value = int(env)
        except ValueError:
            value = 0
        if value > 0:
            return value
        print(f"warning: ignoring LSW1_JOBS={env!r}; expected a positive integer", file=sys.stderr)
    cpus = cpus or os.cpu_count() or 4
    return max(FLOOR, cpus // active_worktrees(root))


def ninja(*args: str) -> list[str]:
    """A ninja command line capped at the shared default."""
    return ["ninja", "-j", str(default_jobs()), *args]


if __name__ == "__main__":
    print(default_jobs())
