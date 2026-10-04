"""Validate artifacts from ARBORETUM_IO_SMOKE_TEST; optionally round-trip Writer.

Usage: python3 packaging/test-odt.py /tmp/Arboretum-IO-XXXXXX [--libreoffice]
"""
import argparse
from pathlib import Path
import os
import subprocess
import tempfile
import xml.etree.ElementTree as ET
import zipfile

NS = {key: f"urn:oasis:names:tc:opendocument:xmlns:{value}:1.0" for key, value in
      (("draw", "drawing"), ("text", "text"), ("style", "style"), ("office", "office"))}


def objects(root):
    assert root.find(".//office:text", NS) is not None, "Not a Writer document"
    assert root.find(".//draw:g", NS) is not None, "Missing editable group"
    assert root.find(".//draw:image", NS) is None, "Diagram flattened to an image"
    lines = root.findall(".//draw:line", NS)
    labels = root.findall(".//draw:rect/text:p", NS)
    assert lines and labels, "Missing native lines or text"
    texts = sorted("".join("".join(p.itertext()).split()) for p in labels
                   if "".join(p.itertext()).strip())
    return len(lines), len(root.findall(".//draw:rect", NS)), texts


def validate(directory, libreoffice=False):
    paths = sorted(directory.glob("Writer-*.odt"))
    assert len(paths) == 8, f"Expected eight ODT fixtures, got {len(paths)}"
    counts = {}
    for path in paths:
        with zipfile.ZipFile(path) as archive:
            assert archive.testzip() is None, "ZIP CRC error"
            first = archive.infolist()[0]
            assert first.filename == "mimetype" and first.compress_type == 0
            assert first.extra == b""
            assert archive.read("mimetype") == b"application/vnd.oasis.opendocument.text"
            for name in ("content.xml", "styles.xml", "META-INF/manifest.xml"):
                ET.fromstring(archive.read(name))
            root = ET.fromstring(archive.read("content.xml"))
        counts[path.stem] = objects(root)
        if path.stem.startswith("Writer-0-"):
            assert root.find(".//text:span[@text:style-name='Overline']", NS) is not None
        if path.stem.startswith("Writer-2-"):
            assert any(line.attrib.get(f"{{{NS['draw']}}}style-name") == "Fraction"
                       for line in root.findall(".//draw:line", NS))
        if path.stem == "Writer-asymmetrisch":
            text = "".join(root.itertext())
            assert "Ä" in text and "Ω" in text and "&%$#_{}~^\\input{evil}" in text
        if path.stem == "Writer-ohne-Beschriftung":
            assert not root.findall(".//draw:line[@draw:style-name='Fraction']", NS)
            # Four nodes remain; probability and result labels are absent.
            assert counts[path.stem][:2] == (4, 8)
    if libreoffice:
        with tempfile.TemporaryDirectory(prefix="arboretum-writer-") as tmp:
            profile = (Path(tmp) / "profile").as_uri()
            subprocess.run(["libreoffice", f"-env:UserInstallation={profile}",
                            "--headless", "--convert-to", "fodt", "--outdir", tmp,
                            *map(str, paths)], check=True, timeout=60,
                           env={**os.environ, "SAL_USE_VCLPLUGIN": "svp",
                                "QT_QPA_PLATFORM": "offscreen"})
            for path in paths:
                root = ET.parse(Path(tmp) / f"{path.stem}.fodt").getroot()
                assert objects(root) == counts[path.stem], f"Lost objects in {path.name}"
    print(f"ODT: {len(paths)} archives/XML/object checks passed"
          + ("; Writer round-trip passed" if libreoffice else ""))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("directory", type=Path)
    parser.add_argument("--libreoffice", action="store_true")
    args = parser.parse_args()
    validate(args.directory, args.libreoffice)
