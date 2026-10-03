"""Run all finite Linux regression suites, retaining logs even after failures."""
import os
import argparse
from pathlib import Path
import signal
import re
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parent.parent
LOGS = ROOT / "dist/test-logs/suite"


def run(name, command, extra_env=None, timeout=180):
    env = {key: value for key, value in os.environ.items()
           if not (key.startswith("ARBORETUM_") and
                   ("TEST" in key or key.startswith("ARBORETUM_FINDER_")))}
    env.update(GSK_RENDERER="cairo", GDK_BACKEND="x11",
               GDK_DISABLE="gl,vulkan", G_DEBUG="fatal-criticals",
               GSETTINGS_BACKEND="memory", GTK_A11Y="none",
               ASAN_OPTIONS="detect_leaks=0:halt_on_error=1",
               UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1")
    env.update(extra_env or {})
    path = LOGS / f"{name}.txt"
    LOGS.mkdir(parents=True, exist_ok=True)
    print(f"START {name}", flush=True)
    with tempfile.TemporaryDirectory(prefix="arboretum-settings-") as settings, path.open("w") as output:
        env["XDG_CONFIG_HOME"] = (extra_env or {}).get("XDG_CONFIG_HOME", settings)
        env["XDG_CACHE_HOME"] = settings
        try:
            process = subprocess.Popen(command, cwd=ROOT, env=env, stdout=output,
                                       stderr=subprocess.STDOUT, start_new_session=True)
            try:
                code = process.wait(timeout=timeout)
            except subprocess.TimeoutExpired:
                os.killpg(process.pid, signal.SIGKILL)
                process.wait()
                output.write(f"\nTIMEOUT after {timeout}s\n")
                code = 124
        except OSError as error:
            output.write(f"\nCould not start test: {error}\n")
            code = 1
        output.write(f"\nExit code: {code}\n")
    # A successful process exit alone does not make GTK diagnostics harmless.
    if re.search(r"(?:Gtk|Gdk|GLib(?:-GObject)?)-(?:WARNING|CRITICAL|ERROR)",
                 path.read_text(errors="replace")):
        with path.open("a") as output:
            output.write("FAIL: GTK/GLib warning or critical diagnostic detected.\n")
        code = 1
    print(f"{'PASS' if code == 0 else 'FAIL'} {name}: {path}", flush=True)
    if code:
        print(path.read_text(errors="replace"), flush=True)
    return code == 0


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--core-only", action="store_true")
    args = parser.parse_args()
    LOGS.mkdir(parents=True, exist_ok=True)
    results = []
    for name in ("KEYBOARD", "IO"):
        results.append((name, run(name, ["./arboretum"],
                        {f"ARBORETUM_{name}_SMOKE_TEST": "1"})))
    for name, binary in (("EDITING", "editing-test"), ("LAYOUT", "layout-test"),
                         ("PARSER", "security-parser-test-bin")):
        results.append((name, run(name, [f"./{binary}"])))
    with tempfile.TemporaryDirectory(prefix="arboretum-deep-suite-") as directory:
        fixture = str(Path(directory) / "deep.bdg")
        generated = run("DEEP-FIXTURE", ["./deep-tree-generator", fixture, "5000"])
        results.append(("DEEP", generated and run("DEEP", ["./arboretum", fixture],
                       {"ARBORETUM_DEEP_TEST_DEPTH": "5000", "TMPDIR": directory})))
    if not args.core_only:
        results.append(("VISUAL", run("VISUAL", [sys.executable, "packaging/test-visual.py"])))
    summary = "\n".join(f"{'PASS' if ok else 'FAIL'} {name}" for name, ok in results)
    summary += f"\n\n{sum(ok for _, ok in results)}/{len(results)} test groups passed.\n"
    (LOGS / "summary.txt").write_text(summary)
    print(summary, flush=True)
    return 0 if all(ok for _, ok in results) else 1


if __name__ == "__main__":
    sys.exit(main())
