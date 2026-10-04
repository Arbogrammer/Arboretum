"""Capture and compare fixed GTK scenes. References are only replaced explicitly."""
import argparse
import hashlib
import html
import json
import importlib.util
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

from PIL import Image, ImageChops, ImageEnhance

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(Path(__file__).resolve().parent))
spec = importlib.util.spec_from_file_location("arboretum_suite", ROOT / "packaging/test-suite.py")
suite = importlib.util.module_from_spec(spec)
spec.loader.exec_module(suite)
REFERENCE = ROOT / "testdaten/visual/reference"
OUTPUT = ROOT / "dist/test-logs/visual"
SCENES = ["mixed-horizontal-inline", "mixed-horizontal-stacked",
          "mixed-vertical-inline", "mixed-vertical-stacked", "entries-horizontal", "entries-vertical", "fractions-horizontal",
          "fractions-vertical", "export-horizontal", "export-vertical", "form-dialog",
          "headings-entries-horizontal", "headings-entries-vertical",
          "headings-export-horizontal", "headings-export-vertical", "headings-dialog"]


def environment():
    versions = {p: subprocess.check_output(["pkg-config", "--modversion", p], text=True).strip()
                for p in ("gtk4", "pango", "cairo", "freetype2", "fontconfig")}
    font = Path(subprocess.check_output(["fc-match", "-f", "%{file}", "DejaVu Sans"], text=True))
    versions["font_sha256"] = hashlib.sha256(font.read_bytes()).hexdigest()
    versions["settings"] = "Adwaita;DejaVu Sans 10;96dpi;scale=1;cairo;x11;1200x900;C.UTF-8"
    return versions


def compare(reference, actual, difference):
    with Image.open(reference) as ref, Image.open(actual) as new:
        ref, new = ref.convert("RGBA"), new.convert("RGBA")
        if ref.size != new.size:
            canvas = Image.new("RGB", (max(ref.width, new.width), max(ref.height, new.height)), "magenta")
            canvas.paste(new, (0, 0))
            canvas.save(difference)
            return False, f"Size changed: {ref.size} -> {new.size}"
        diff = ImageChops.difference(ref, new)
        # Ignore only tiny antialiasing noise; no changed-pixel percentage allowance.
        channels = diff.split()
        maximum = channels[0]
        for channel in channels[1:]:
            maximum = ImageChops.lighter(maximum, channel)
        mask = maximum.point(lambda value: 255 if value > 8 else 0)
        changed = mask.histogram()[255]
        if changed:
            marked = ImageEnhance.Brightness(new.convert("RGB")).enhance(0.35)
            marked.paste((255, 0, 100), mask=mask)
            marked.save(difference)
        return changed == 0, f"{changed} pixels differ by more than 8/255"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--update", action="store_true", help="Explicitly replace reference images after review")
    args = parser.parse_args()
    OUTPUT.mkdir(parents=True, exist_ok=True)
    (OUTPUT / "index.html").write_text('<!doctype html><meta charset="utf-8">'
        '<h1>Visual comparison incomplete</h1><p>See ../suite/VISUAL-CAPTURE.txt for the capture log.</p>')
    actual = OUTPUT / "actual"
    actual.mkdir(exist_ok=True)
    for name in SCENES:
        (actual / f"{name}.png").unlink(missing_ok=True)
    with tempfile.TemporaryDirectory(prefix="arboretum-visual-settings-") as settings:
        env = {"ARBORETUM_VISUAL_OUTPUT": str(actual), "GSETTINGS_BACKEND": "memory",
               "XDG_CONFIG_HOME": settings, "GTK_THEME": "Adwaita", "GDK_SCALE": "1",
               "GDK_DPI_SCALE": "1", "LC_ALL": "C.UTF-8", "GTK_CSD": "0",
               "FONTCONFIG_FILE": str(ROOT / "testdaten/visual/fonts.conf")}
        if not suite.run("VISUAL-CAPTURE", ["./visual-test"], env):
            return 1
    metadata = environment()
    (actual / "environment.json").write_text(json.dumps(metadata, indent=2) + "\n")
    missing = [name for name in SCENES if not (actual / f"{name}.png").exists()]
    if missing:
        print(f"Missing captures: {missing}")
        return 1
    if args.update:
        REFERENCE.mkdir(parents=True, exist_ok=True)
        for name in SCENES:
            shutil.copy2(actual / f"{name}.png", REFERENCE)
        shutil.copy2(actual / "environment.json", REFERENCE)
    reference_metadata = REFERENCE / "environment.json"
    compatible = reference_metadata.exists() and json.loads(reference_metadata.read_text()) == metadata
    rows, failures = [], 0
    for name in SCENES:
        ref, new = REFERENCE / f"{name}.png", actual / f"{name}.png"
        diff = OUTPUT / f"{name}-diff.png"
        diff.unlink(missing_ok=True)
        copied_ref = OUTPUT / f"{name}-reference.png"
        if ref.exists():
            shutil.copy2(ref, copied_ref)
            ok, detail = compare(ref, new, diff)
        else:
            ok, detail = False, "Missing reference; create and review with make visual-reference"
        failures += not ok
        print(f"{'PASS' if ok else 'FAIL'} {name}: {detail}")
        images = f'<figure><img src="{name}-reference.png"><figcaption>Reference</figcaption></figure>' if ref.exists() else ""
        images += f'<figure><img src="actual/{name}.png"><figcaption>Current</figcaption></figure>'
        if diff.exists():
            images += f'<figure><img src="{name}-diff.png"><figcaption>Differences</figcaption></figure>'
        rows.append(f'<h2>{name}: {"PASS" if ok else "FAIL"}</h2><p>{html.escape(detail)}</p><div>{images}</div>')
    status = "Environment matches reference." if compatible else "FAIL: Rendering environment differs or has no reference. Use the documented package versions; do not automatically update references."
    (OUTPUT / "index.html").write_text('<!doctype html><meta charset="utf-8"><title>Arboretum visual tests</title>'
        '<style>body{font:16px sans-serif;margin:24px;background:#f5f5f5}div{display:flex;gap:16px}figure{margin:0;flex:1;min-width:0}img{width:100%;border:1px solid #bbb}h2{margin-top:40px}</style>'
        f'<h1>Arboretum visual tests</h1><p>{status}</p>' + ''.join(rows))
    print(status)
    print(f"Report: {OUTPUT / 'index.html'}")
    return int(bool(failures or not compatible))


if __name__ == "__main__":
    sys.exit(main())
