#!/usr/bin/env bash
set -euo pipefail

root_dir=$(cd "$(dirname "$0")/../.." && pwd)
prefix=$(brew --prefix)
bundle="$root_dir/dist/Arboretum.app"
macos_dir="$bundle/Contents/MacOS"
frameworks_dir="$bundle/Contents/Frameworks"
resources_dir="$bundle/Contents/Resources"

rm -rf "$bundle"
mkdir -p "$macos_dir" "$frameworks_dir" "$resources_dir/share"

gcc -O2 -Wall -Wextra -Wno-deprecated-declarations -Wno-unused-parameter \
  $(pkg-config --cflags gtk4) "$root_dir/baumg.c" -o "$macos_dir/Arboretum-bin" \
  $(pkg-config --libs gtk4) -lm

# Copy all Homebrew dynamic libraries used by the executable, including their
# transitive dependencies. Apple system frameworks deliberately stay external.
while :; do
  added=0
  for target in "$macos_dir/Arboretum-bin" "$frameworks_dir"/*; do
    [ -f "$target" ] || continue
    while IFS= read -r library; do
      case "$library" in
        "$prefix"/*)
          name=$(basename "$library")
          destination="$frameworks_dir/$name"
          if [ ! -e "$destination" ]; then
            cp -L "$library" "$destination"
            added=1
          fi
          ;;
      esac
    done < <(otool -L "$target" | tail -n +2 | awk '{print $1}')
  done
  [ "$added" -eq 0 ] && break
done

# Make each bundled library resolve its neighbours from inside the app bundle.
for target in "$macos_dir/Arboretum-bin" "$frameworks_dir"/*; do
  [ -f "$target" ] || continue
  if [ "$target" = "$macos_dir/Arboretum-bin" ]; then
    replacement='@executable_path/../Frameworks'
  else
    replacement='@loader_path'
  fi
  otool -L "$target" | tail -n +2 | awk '{print $1}' | while IFS= read -r library; do
    case "$library" in
      "$prefix"/*)
        install_name_tool -change "$library" "$replacement/$(basename "$library")" "$target"
        ;;
    esac
  done
done

cp -a "$prefix/share/glib-2.0" "$resources_dir/share/"
cp -a "$prefix/share/gtk-4.0" "$resources_dir/share/"
cp -a "$prefix/share/icons" "$resources_dir/share/"
cp -a "$root_dir/arboretum-icon.png" "$resources_dir/arboretum-icon.png"

cat > "$macos_dir/Arboretum" <<'EOF'
#!/bin/sh
set -eu
bundle_dir=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
export GSETTINGS_SCHEMA_DIR="$bundle_dir/Resources/share/glib-2.0/schemas"
export XDG_DATA_DIRS="$bundle_dir/Resources/share"
exec "$bundle_dir/MacOS/Arboretum-bin" "$@"
EOF
chmod +x "$macos_dir/Arboretum"

cat > "$bundle/Contents/Info.plist" <<'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0"><dict>
  <key>CFBundleDisplayName</key><string>Arboretum</string>
  <key>CFBundleExecutable</key><string>Arboretum</string>
  <key>CFBundleIdentifier</key><string>org.arbogrammer.arboretum</string>
  <key>CFBundleName</key><string>Arboretum</string>
  <key>CFBundlePackageType</key><string>APPL</string>
  <key>CFBundleShortVersionString</key><string>2.0</string>
  <key>CFBundleVersion</key><string>2.0</string>
</dict></plist>
EOF

# Re-sign after install_name_tool changed bundled Homebrew libraries.  This is
# an ad-hoc signature only; notarization still requires an Apple developer ID.
codesign --force --deep --sign - "$bundle"

rm -f "$root_dir/dist/Arboretum-macOS-x64.zip"
cd "$root_dir/dist"
zip -qry Arboretum-macOS-x64.zip Arboretum.app
