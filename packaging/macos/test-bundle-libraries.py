"""Resolver tests can also run on Linux; Mach-O audit runs in the macOS build."""
from pathlib import Path
import runpy
import tempfile
import unittest

helpers = runpy.run_path(str(Path(__file__).with_name("bundle-libraries.py")))


class ResolutionTest(unittest.TestCase):
    def test_homebrew_relative_dependencies(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            libs = root / "Cellar/webp/1.0/lib"
            libs.mkdir(parents=True)
            loader = libs / "libwebp.7.dylib"
            dependency = libs / "libsharpyuv.0.dylib"
            loader.touch()
            dependency.touch()
            executable = root / "App/Contents/MacOS/Arboretum-bin"
            search = [helpers["expand"]("@loader_path/../lib", loader, executable)]
            for name in (str(dependency), "@loader_path/libsharpyuv.0.dylib",
                         "@rpath/libsharpyuv.0.dylib"):
                self.assertEqual(helpers["resolve"](name, loader, executable, search, root), dependency)
            with self.assertRaises(RuntimeError):
                helpers["resolve"]("@rpath/missing.dylib", loader, executable, search, root)

    def test_system_libraries_stay_external(self):
        self.assertTrue(helpers["system_library"]("/usr/lib/libSystem.B.dylib"))
        self.assertTrue(helpers["system_library"]("/System/Library/Frameworks/AppKit.framework/AppKit"))
        self.assertFalse(helpers["system_library"]("/usr/local/lib/libglib.dylib"))


if __name__ == "__main__":
    unittest.main()
