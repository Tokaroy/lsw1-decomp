import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))

import sweep  # noqa: E402

prototypes = sys.modules[sweep.fix_prototypes.__module__]


class PrototypeClassTest(unittest.TestCase):
    def fix(self, body):
        old = prototypes._PROTOS
        prototypes._PROTOS = {"g": ("void", ["void *", "s32"])}
        try:
            return sweep.fix_prototypes(body, "f", "\tbl g\n", "")
        finally:
            prototypes._PROTOS = old

    def test_used_result_keeps_value_type(self):
        # fn_8002CD7C is defined `void` but forwards r3; fn_8007BD64 stores its result.
        out = self.fix("u16 g(s32, s32);\n\nvoid f(u16 *p) {\n    *p = g(1, 0);\n}\n")
        self.assertIn("u16 g(void *, s32);\n", out)

    def test_statement_call_stays_void(self):
        out = self.fix("void g(s32, s32);\n\nvoid f(void) {\n    g(1, 0);\n}\n")
        self.assertIn("void g(void *, s32);\n", out)

    def test_own_prototype_names_by_register_class(self):
        # fn_8007BE20 declares fn_8007AC80(void *, f32 x6, s32, u8 x4); m2c names the
        # definition's parameters arg0..arg5 (r3..r8) and farg0..farg5 (f1..f6).
        body = ("void f(void *arg0, s32 arg1, u8 arg2, f32 farg0, f32 farg1) {\n"
                "    g(arg0, arg1 + arg2, farg0 + farg1);\n}\n")
        out = sweep.adopt_own_prototype(body, "f", "void f(void *, f32, f32, s32, u8, u8);\n")
        self.assertIn("void f(void *arg0, f32 farg0, f32 farg1, s32 arg1, u8 arg2, u8 pad5) {", out)

    def test_own_void_pointer_keeps_m2c_pointer_type(self):
        body = "s32 f(u8 *arg0) {\n    return *arg0;\n}\n"
        out = sweep.adopt_own_prototype(body, "f", "s32 f(void *);\n")
        self.assertIn("s32 f(void *arg0_v) {\n    u8 *arg0 = arg0_v;\n    return *arg0;", out)


if __name__ == "__main__":
    unittest.main()
