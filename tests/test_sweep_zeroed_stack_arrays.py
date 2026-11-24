"""Regression tests for m2c locals cleared by memset before indexed accesses."""

import textwrap
import unittest

from tools import sweep  # noqa: F401
from sweep_repairs.zeroed_stack_arrays import zeroed_stack_arrays


BODY = textwrap.dedent("""\
    void f(s32 i) {
        memset((s32 (*)[]) &sp8, 0, 0x200);
        if ((&sp8)[i] == 0) {
            (&sp8)[i] = 1;
        }
        memset((s32 (*)[]) &sp8, 0, 0x200);
    }
    """)


class ZeroedStackArraysTests(unittest.TestCase):
    def test_declares_one_array_for_repeated_clear(self):
        result = zeroed_stack_arrays(BODY)
        self.assertIn("    s32 sp8[128];", result)
        self.assertEqual(result.count("memset(sp8, 0, 0x200);"), 2)
        self.assertIn("if (sp8[i] == 0)", result)
        self.assertIn("sp8[i] = 1;", result)

    def test_preserves_existing_declaration(self):
        declared = BODY.replace("void f(s32 i) {", "void f(s32 i) {\n    s32 sp8;")
        self.assertEqual(zeroed_stack_arrays(declared), declared)

    def test_requires_agreement_about_array_size(self):
        inconsistent = BODY.replace("0x200);\n}", "0x208);\n}")
        self.assertEqual(zeroed_stack_arrays(inconsistent), inconsistent)


if __name__ == "__main__":
    unittest.main()
