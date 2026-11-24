import os
import sys
import tempfile
import time
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
import jobs  # noqa: E402


class DefaultJobsTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name)
        self.trees = [self.root / f"wt{i}" for i in range(4)]
        for tree in self.trees:
            tree.mkdir(parents=True)
        patcher = mock.patch.object(jobs, "_worktrees", return_value=self.trees)
        patcher.start()
        self.addCleanup(patcher.stop)
        self.addCleanup(self.tmp.cleanup)

    def touch(self, tree, age):
        log = tree / ".ninja_log"
        log.write_text("")
        stamp = time.time() - age
        os.utime(log, (stamp, stamp))

    def test_env_overrides(self):
        with mock.patch.dict(os.environ, {"LSW1_JOBS": "3"}):
            self.assertEqual(jobs.default_jobs(self.trees[0], cpus=16), 3)

    def test_bad_env_falls_back(self):
        with mock.patch.dict(os.environ, {"LSW1_JOBS": "lots"}):
            self.assertEqual(jobs.default_jobs(self.trees[0], cpus=16), 16)

    def test_splits_cores_across_recent_worktrees(self):
        self.touch(self.trees[1], 60)
        self.touch(self.trees[2], 60)
        self.touch(self.trees[3], jobs.ACTIVE_WINDOW + 600)  # idle: not counted
        with mock.patch.dict(os.environ, {"LSW1_JOBS": ""}):
            self.assertEqual(jobs.active_worktrees(self.trees[0]), 3)
            self.assertEqual(jobs.default_jobs(self.trees[0], cpus=16), 5)

    def test_floor(self):
        for tree in self.trees[1:]:
            self.touch(tree, 0)
        with mock.patch.dict(os.environ, {"LSW1_JOBS": ""}):
            self.assertEqual(jobs.default_jobs(self.trees[0], cpus=4), jobs.FLOOR)

    def test_ninja_command(self):
        with mock.patch.dict(os.environ, {"LSW1_JOBS": "6"}):
            self.assertEqual(jobs.ninja("a.o"), ["ninja", "-j", "6", "a.o"])


if __name__ == "__main__":
    unittest.main()
