"""Resolve Mach-O dependencies at their original locations before relocating.

Homebrew libraries may use @rpath or @loader_path, not just absolute paths.
Fail rather than produce an app that still depends on the build machine.
"""
from pathlib import Path
import shutil
import os
import stat
import subprocess
import sys


def output(*args):
    return subprocess.check_output(args, text=True)


def dependencies(image):
    return [line.strip().split(" (", 1)[0]
            for line in output("otool", "-L", str(image)).splitlines()[1:]]


def rpaths(image):
    lines = output("otool", "-l", str(image)).splitlines()
    paths = []
    for i, line in enumerate(lines):
        if line.strip() == "cmd LC_RPATH":
            for following in lines[i + 1:i + 4]:
                if following.strip().startswith("path "):
                    paths.append(following.strip()[5:].split(" (offset", 1)[0])
    return paths


def system_library(name):
    return name.startswith(("/usr/lib/", "/System/Library/"))


def expand(name, loader, executable):
    return Path(name.replace("@loader_path", str(loader.parent))
                    .replace("@executable_path", str(executable.parent)))


def resolve(name, loader, executable, search_paths, prefix):
    if name.startswith("@rpath/"):
        suffix = name[len("@rpath/"):]
        candidates = [path / suffix for path in search_paths]
        candidates += [loader.parent / suffix, prefix / "lib" / suffix]
    else:
        candidates = [expand(name, loader, executable)]
    for candidate in candidates:
        if candidate.is_file():
            return candidate.resolve()
    raise RuntimeError(f"Cannot resolve {name} required by {loader}; tried {candidates}")


def bundle(executable, destination, prefix, *extra_images):
    executable, destination, prefix = map(Path, (executable, destination, prefix))
    executable = executable.resolve()
    queue = [(executable, executable, [])]
    copied = {}
    # Runtime-loaded pixbuf modules are invisible to otool's dependency walk.
    for image in extra_images:
        source = Path(image).resolve()
        target = destination / source.name
        shutil.copy2(source, target)
        target.chmod(target.stat().st_mode | stat.S_IWUSR)
        copied[source.name] = source
        queue.append((source, target, []))
    for original, target, inherited in queue:
        search_paths = [expand(path, original, executable) for path in rpaths(original)] + inherited
        changes = []
        for name in dependencies(original):
            if system_library(name):
                continue
            source = resolve(name, original, executable, search_paths, prefix)
            if source == original:
                continue  # LC_ID_DYLIB is also printed by otool -L.
            library_target = destination / source.name
            previous = copied.get(source.name)
            if previous is not None and previous != source:
                raise RuntimeError(f"Conflicting library names: {previous} and {source}")
            if previous is None:
                copied[source.name] = source
                shutil.copy2(source, library_target)
                library_target.chmod(library_target.stat().st_mode | stat.S_IWUSR)
                queue.append((source, library_target, search_paths))
            relative = "@loader_path/" + os.path.relpath(destination, target.parent) + "/"
            changes += ["-change", name, relative + source.name]
        command = ["install_name_tool"]
        is_library = target.suffix in (".dylib", ".so")
        if is_library:
            command += ["-id", "@rpath/" + target.name]
        if changes or is_library:
            subprocess.run(command + changes + [str(target)], check=True)

    # Audit the output independently of the resolver. No Homebrew path or
    # unresolved run-path dependency may remain in a load command.
    for _, target, _ in queue:
        for name in dependencies(target):
            if target != executable and name == "@rpath/" + target.name:
                continue  # library's own ID, not an imported dependency
            if system_library(name):
                continue
            if not name.startswith(("@loader_path/", "@executable_path/")):
                raise RuntimeError(f"Unrelocated dependency {name} in {target}")
            if not expand(name, target, executable).is_file():
                raise RuntimeError(f"Missing bundled dependency {name} in {target}")
    print(f"Bundled and checked {len(copied)} libraries")


if __name__ == "__main__":
    bundle(*sys.argv[1:])
