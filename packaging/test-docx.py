"""Check native editable DOCX exports against the ODT geometry fixtures.

Run after ARBORETUM_IO_SMOKE_TEST. Optional --soffice /absolute/path/to/soffice
also verifies importing, modifying and saving with LibreOffice.
"""
import argparse
import math
import os
from pathlib import Path
import re
import subprocess
import tempfile
import xml.etree.ElementTree as ET
import zipfile

NS = {
    "w": "http://schemas.openxmlformats.org/wordprocessingml/2006/main",
    "v": "urn:schemas-microsoft-com:vml",
    "rel": "http://schemas.openxmlformats.org/package/2006/relationships",
    "ct": "http://schemas.openxmlformats.org/package/2006/content-types",
    "draw": "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0",
    "svg": "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0",
    "text": "urn:oasis:names:tc:opendocument:xmlns:text:1.0",
}


def attr(element, prefix, name):
    return element.attrib[f"{{{NS[prefix]}}}{name}"]


def style(element):
    return dict(part.split(":", 1) for part in element.attrib["style"].split(";") if part)


def word_text(element):
    return "".join(t.text or "" for t in element.findall(".//w:t", NS))


def signature(root):
    group = root.find(".//v:group", NS)
    assert group is not None, "Missing editable drawing group"
    lines = group.findall("v:line", NS)
    boxes = group.findall("v:rect", NS)
    texts = [word_text(box) for box in boxes if box.find("v:textbox", NS) is not None]
    return len(lines), len(boxes), sorted(t for t in texts if t)


def validate(path):
    with zipfile.ZipFile(path) as archive:
        assert archive.testzip() is None, "ZIP CRC error"
        entries = archive.namelist()
        assert len(entries) == len(set(entries)), "Duplicate ZIP entries"
        assert not any("media/" in n or "vba" in n.lower() for n in entries)
        parts = {name: ET.fromstring(archive.read(name)) for name in entries}
    rels = parts["_rels/.rels"].findall("rel:Relationship", NS)
    assert len(rels) == 1 and rels[0].get("Target") == "word/document.xml"
    assert rels[0].get("Type").endswith("/officeDocument")
    for name, root in parts.items():
        if name.endswith(".rels"):
            base = Path() if name == "_rels/.rels" else Path(name).parent.parent
            for rel in root:
                assert rel.get("TargetMode") != "External"
                assert (base / rel.get("Target")).as_posix() in parts
    types = parts["[Content_Types].xml"]
    main = types.find("ct:Override[@PartName='/word/document.xml']", NS)
    assert main is not None and main.get("ContentType").endswith(".document.main+xml")
    root = parts["word/document.xml"]
    group = root.find(".//v:group", NS)
    assert group is not None
    ids = [x.attrib["id"] for x in group.iter() if "id" in x.attrib]
    assert len(ids) == len(set(ids)), "Shape IDs must be unique"
    page = root.find(".//w:pgSz", NS)
    assert all(0 < int(attr(page, "w", d)) <= 31680 for d in ("w", "h"))
    # Native line endpoints must match the established ODT geometry (including
    # descending/vertical branches and separate fraction bars).
    odt = path.with_name(path.name.replace("Word-", "Writer-")).with_suffix(".odt")
    with zipfile.ZipFile(odt) as archive:
        reference = ET.fromstring(archive.read("content.xml"))
    lines = group.findall("v:line", NS)
    refs = reference.findall(".//draw:line", NS)
    assert len(lines) == len(refs) > 0
    for actual, expected in zip(lines, refs):
        values = [float(v) for key in ("from", "to") for v in actual.attrib[key].split(",")]
        points = [(float(attr(expected, "svg", key)[:-2]) - 1) * 360000
                  for key in ("x1", "y1", "x2", "y2")]
        assert all(abs(a - b) < 1 for a, b in zip(values, points)), (path, values, points)
    actual_boxes = group.findall("v:rect", NS)
    reference_boxes = reference.findall(".//draw:rect", NS)
    assert len(actual_boxes) == len(reference_boxes)
    for box, ref in zip(actual_boxes, reference_boxes):
        if box.find("v:textbox", NS) is None:
            continue
        transform = ref.get(f"{{{NS['draw']}}}transform")
        angle = -float(re.search(r"rotate \(([^)]+)\)", transform)[1]) * 180 / math.pi
        assert abs(float(style(box)["rotation"]) - angle) < .0001
        assert box.find(".//w:t", NS) is not None
    texts = signature(root)[2]
    if path.stem.startswith("Word-0-"):
        assert any("Ä\u0305pfel Ω" in t for t in texts), "Lost editable overline"
    if path.stem.startswith("Word-2-"):
        assert any(x.get("id").startswith("Bruchstrich_") for x in lines)
    if path.stem == "Word-asymmetrisch":
        assert any("Ä Ω & % $ # _ { } ~ ^ \\input{evil}" in t for t in texts)
    if path.stem == "Word-ohne-Beschriftung":
        assert signature(root)[:2] == (4, 8)
        assert len(group.findall("v:rect/v:textbox", NS)) == 4, "Hidden probabilities/results were exported"
    return root


def roundtrip(paths, roots, soffice):
    with tempfile.TemporaryDirectory(prefix="arboretum-docx-") as tmp:
        tmp = Path(tmp)
        # Edit actual native text and a line in a copy, then import/save it.
        edited = tmp / "Edited.docx"
        source = paths[0]
        with zipfile.ZipFile(source) as archive, zipfile.ZipFile(edited, "w") as output:
            for name in archive.namelist():
                data = archive.read(name)
                if name == "word/document.xml":
                    root = ET.fromstring(data)
                    root.find(".//v:textbox//w:t", NS).text = "Bearbeitet Ω & < >"
                    line = root.find(".//v:line", NS)
                    point = line.get("from").split(",")
                    point[0] = str(int(point[0]) + 360000)
                    line.set("from", ",".join(point))
                    data = ET.tostring(root, encoding="utf-8", xml_declaration=True)
                    edited_signature = signature(root)
                    edited_start = float(point[0]) + 360000
                output.writestr(name, data)
        env = {**os.environ, "HOME": str(tmp), "XDG_CACHE_HOME": str(tmp / "cache")}
        subprocess.run([str(soffice), f"-env:UserInstallation={(tmp / 'profile').as_uri()}",
                        "--headless", "--convert-to", "fodt", "--outdir", str(tmp),
                        *map(str, paths), str(edited)], check=True, timeout=60, env=env)
        for path in [*paths, edited]:
            result = ET.parse(tmp / f"{path.stem}.fodt").getroot()
            group = result.find(".//draw:g", NS)
            assert group is not None, f"Lost group: {path.name}"
            assert result.find(".//draw:image", NS) is None, "Flattened to a picture"
            lines = group.findall("draw:line", NS)
            shapes = [x for x in group if x.tag != f"{{{NS['draw']}}}line"]
            texts = ["".join(x.itertext()).strip() for shape in shapes
                     for x in shape.findall(".//text:p", NS)]
            expected = edited_signature if path == edited else signature(roots[path])
            # Writer may split runs or serialize text:s instead of spaces.
            normalize = lambda values: sorted("".join(v.split()) for v in values if v.strip())
            assert len(lines) == expected[0] and len(shapes) == expected[1]
            assert normalize(texts) == normalize(expected[2]), f"Lost text: {path.name}"
            if path == edited:
                value = attr(lines[0], "svg", "x1")
                match = re.fullmatch(r"([-0-9.]+)(cm|mm|in|pt)", value)
                assert match, value
                units = {"cm": 360000, "mm": 36000, "in": 914400, "pt": 12700}
                actual_start = float(match[1]) * units[match[2]]
                assert abs(actual_start - edited_start) < 1000, "Edited line endpoint was lost"
        print("DOCX: native objects and edited text survive the Writer round-trip")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("directory", type=Path)
    parser.add_argument("--soffice", type=Path)
    args = parser.parse_args()
    paths = sorted(args.directory.resolve().glob("Word-*.docx"))
    assert len(paths) == 8, f"Expected eight DOCX fixtures, got {len(paths)}"
    roots = {path: validate(path) for path in paths}
    print("DOCX: 8 archives, native shapes, text and ODT geometry comparisons passed")
    if args.soffice:
        roundtrip(paths, roots, args.soffice.resolve())
