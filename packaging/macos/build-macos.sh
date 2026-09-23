#!/usr/bin/env bash
set -euo pipefail

root_dir=$(cd "$(dirname "$0")/../.." && pwd)
source "$root_dir/packaging/version.sh"
prefix=$(brew --prefix)
bundle="$root_dir/dist/Arboretum.app"
macos_dir="$bundle/Contents/MacOS"
frameworks_dir="$bundle/Contents/Frameworks"
resources_dir="$bundle/Contents/Resources"

rm -rf "$bundle"
mkdir -p "$macos_dir" "$frameworks_dir" "$resources_dir/share"

gcc -O2 -Wall -Wextra -Wno-deprecated-declarations -Wno-unused-parameter \
  -DARBORETUM_BUILD_ID="\"$build_id\"" \
  $(pkg-config --cflags gtk4) "$root_dir/baumg.c" -o "$macos_dir/Arboretum-bin" \
  $(pkg-config --libs gtk4) -lm

# Resolve relative Homebrew dependencies as well as absolute ones, then audit
# every rewritten load command before signing the app.
python3 "$root_dir/packaging/macos/bundle-libraries.py" \
  "$macos_dir/Arboretum-bin" "$frameworks_dir" "$prefix" \
  "$(pkg-config --variable=gdk_pixbuf_query_loaders gdk-pixbuf-2.0)" \
  "$(pkg-config --variable=gdk_pixbuf_moduledir gdk-pixbuf-2.0)"/*.so

cp -a "$prefix/share/glib-2.0" "$resources_dir/share/"
cp -a "$prefix/share/gtk-4.0" "$resources_dir/share/"
cp -a "$prefix/share/icons" "$resources_dir/share/"
cp -a "$root_dir/arboretum-icon.png" "$resources_dir/arboretum-icon.png"
arboretum_write_build_info "$resources_dir/BUILD-INFO.txt"

cat > "$macos_dir/Arboretum" <<'EOF'
#!/bin/sh
set -eu
bundle_dir=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
export GSETTINGS_SCHEMA_DIR="$bundle_dir/Resources/share/glib-2.0/schemas"
export XDG_DATA_DIRS="$bundle_dir/Resources/share"
# Generate paths at launch: the app can be moved or placed in a Unicode path.
# Never use Homebrew's loader cache, which would load a second pixbuf library.
cache=$(mktemp "${TMPDIR:-/tmp}/arboretum-pixbuf.XXXXXX")
trap 'rm -f "$cache"' EXIT HUP INT TERM
export GDK_PIXBUF_MODULEDIR="$bundle_dir/Frameworks"
export GDK_PIXBUF_MODULE_FILE="$cache"
"$bundle_dir/Frameworks/gdk-pixbuf-query-loaders" "$bundle_dir"/Frameworks/libpixbufloader-*.so > "$cache"
"$bundle_dir/MacOS/Arboretum-bin" "$@"
EOF
chmod +x "$macos_dir/Arboretum"

cat > "$bundle/Contents/Info.plist" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0"><dict>
  <key>CFBundleDisplayName</key><string>Arboretum</string>
  <key>CFBundleExecutable</key><string>Arboretum</string>
  <key>CFBundleIdentifier</key><string>org.arbogrammer.arboretum</string>
  <key>CFBundleName</key><string>Arboretum</string>
  <key>CFBundlePackageType</key><string>APPL</string>
  <key>CFBundleShortVersionString</key><string>$arboretum_version</string>
  <key>CFBundleVersion</key><string>$arboretum_version</string>
</dict></plist>
EOF

# Re-sign after install_name_tool changed bundled Homebrew libraries.  This is
# an ad-hoc signature only; notarization still requires an Apple developer ID.
codesign --force --deep --sign - "$bundle"

rm -f "$root_dir/dist/Arboretum-macOS-x64.zip"
cd "$root_dir/dist"
zip -qry Arboretum-macOS-x64.zip Arboretum.app
