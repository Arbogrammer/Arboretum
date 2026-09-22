"""Test a packaged Unix executable, retaining output for the CI artifact."""
import os
from pathlib import Path
import signal
import subprocess
import sys

executable = str(Path(sys.argv[1]).resolve())
logs = Path("dist/test-logs")
logs.mkdir(parents=True, exist_ok=True)
failed = False
for test in ("KEYBOARD", "IO"):
    env = os.environ.copy()
    for name in ("ARBORETUM_KEYBOARD_SMOKE_TEST", "ARBORETUM_IO_SMOKE_TEST"):
        env.pop(name, None)
    env.update({f"ARBORETUM_{test}_SMOKE_TEST": "1", "ARBORETUM_DIAGNOSTIC": "1",
                "GSK_RENDERER": "cairo", "APPIMAGE_EXTRACT_AND_RUN": "1"})
    with (logs / f"{test}.txt").open("w") as output:
        try:
            process = subprocess.Popen([executable], env=env, stdout=output,
                                       stderr=subprocess.STDOUT, start_new_session=True)
            code = process.wait(timeout=90)
            failed |= code != 0
            print(f"{test} exit code: {code}", file=output)
        except subprocess.TimeoutExpired:
            os.killpg(process.pid, signal.SIGKILL)
            process.wait()
            failed = True
            print(f"{test}: timeout", file=output)
    print((logs / f"{test}.txt").read_text())
sys.exit(1 if failed else 0)
