"""Exercise LaunchServices document events, not command-line file arguments."""
from pathlib import Path
import re
import subprocess
import tempfile
import time

app = Path("dist/Arboretum.app").resolve()
logs = Path("dist/test-logs")
match = re.search(r"IO-Test Ausgabeordner: (.+)", (logs / "IO.txt").read_text())
assert match, "Missing IO-test fixture"
document = Path(match[1].strip()) / "Bäume und Grüße Ω/Äpfel Öl Übung.bdg"
assert document.is_file()
with tempfile.TemporaryDirectory(prefix="arboretum-finder-") as folder:
    for mode in ("cold", "running"):
        result = Path(folder) / mode
        env = {
            # Match the runtime suite: hosted Intel runners have no usable GPU.
            "GSK_RENDERER": "cairo",
            "ARBORETUM_STARTUP_SMOKE_TEST": "1",
            "ARBORETUM_FINDER_SMOKE_TEST": "1",
            "ARBORETUM_STARTUP_EXPECTED_PATH": str(document),
            "ARBORETUM_FINDER_RESULT": str(result),
            "ARBORETUM_DIAGNOSTIC": "1",
        }
        command = ["/usr/bin/open", "-n", "-W", "-a", str(app)]
        trace = (logs / f"Finder-{mode}.txt").resolve()
        command += ["--stdout", str(trace), "--stderr", str(trace)]
        for key, value in env.items():
            command += ["--env", f"{key}={value}"]
        if mode == "cold":
            command.append(str(document))
        process = subprocess.Popen(command)
        try:
            if mode == "running":
                deadline = time.monotonic() + 20
                while not trace.exists() or "entering event loop" not in trace.read_text():
                    assert time.monotonic() < deadline, "App did not become ready"
                    time.sleep(0.1)
                subprocess.run(["/usr/bin/open", "-a", str(app), str(document)],
                               check=True, timeout=15)
            assert process.wait(timeout=40) == 0
            assert result.read_text() == "ok", f"Finder {mode}: wrong document"
            print(f"Finder {mode}: Unicode document loaded correctly", flush=True)
        finally:
            if process.poll() is None:
                pids = subprocess.run(["pgrep", "-f", str(app / "Contents/MacOS/Arboretum-bin")],
                                      capture_output=True, text=True).stdout.split()
                for pid in pids:
                    sample = logs / f"Finder-{mode}-{pid}-sample.txt"
                    subprocess.run(["/usr/bin/sample", pid, "2", "-file", str(sample)], timeout=15)
                    if sample.exists(): print(sample.read_text()[:18000], flush=True)
            print(f"Finder {mode} result: {result.read_text() if result.exists() else 'missing'}", flush=True)
            if trace.exists():
                print(trace.read_text(), flush=True)
            if process.poll() is None:
                process.kill()
                process.wait()
