#!/usr/bin/env bash
set -euo pipefail
root_dir=$(cd "$(dirname "$0")/../.." && pwd)
source "$root_dir/packaging/version.sh"
[[ $(uname -m) == x86_64 ]] || { echo 'This package targets x86_64' >&2; exit 1; }
mkdir -p "$root_dir/dist"
work_dir=$(mktemp -d "$root_dir/dist/linux-build.XXXXXX")
appdir="$work_dir/AppDir"
mkdir -p "$appdir/usr/bin" "$appdir/usr/lib/gdk-pixbuf-loaders" \
  "$appdir/usr/lib/gio/modules" "$appdir/usr/share/arboretum" \
  "$appdir/usr/share/glib-2.0" "$appdir/usr/share/icons"
gcc -O2 -g -Wall -Wextra -Wno-deprecated-declarations -Wno-unused-parameter \
  -DARBORETUM_BUILD_ID="\"$build_id\"" \
  $(pkg-config --cflags gtk4) "$root_dir/baumg.c" -o "$appdir/usr/bin/arboretum" \
  $(pkg-config --libs gtk4) -lm

# Keep the system C runtime/loader external. Bundle the GTK dependency closure
# and pixbuf plugins; copying an old AppDir would hide stale dependencies.
copy_dependencies() {
  local library name dependencies
  dependencies=$(ldd "$1")
  if [[ "$dependencies" == *'not found'* ]]; then
    printf '%s\n' "$dependencies" >&2
    exit 1
  fi
  while IFS= read -r library; do
    name=$(basename "$library")
    case "$name" in
      ld-linux*|libc.so.*|libm.so.*|libdl.so.*|libpthread.so.*|librt.so.*|libresolv.so.*|libutil.so.*) continue ;;
    esac
    cp -L "$library" "$appdir/usr/lib/$name"
  done < <(printf '%s\n' "$dependencies" | awk '$2 == "=>" && $3 ~ /^\// {print $3}')
}
copy_dependencies "$appdir/usr/bin/arboretum"
query_loaders=$(pkg-config --variable=gdk_pixbuf_query_loaders gdk-pixbuf-2.0)
cp "$query_loaders" "$appdir/usr/bin/gdk-pixbuf-query-loaders"
copy_dependencies "$query_loaders"
loader_dir=$(pkg-config --variable=gdk_pixbuf_moduledir gdk-pixbuf-2.0)
for loader in "$loader_dir"/*.so; do
  [[ -f "$loader" ]] || continue
  cp -L "$loader" "$appdir/usr/lib/gdk-pixbuf-loaders/"
  copy_dependencies "$loader"
done
cp -a /usr/share/glib-2.0/schemas "$appdir/usr/share/glib-2.0/"
cp -a /usr/share/gtk-4.0 "$appdir/usr/share/"
for theme in Adwaita hicolor; do
  [[ ! -d "/usr/share/icons/$theme" ]] || cp -a "/usr/share/icons/$theme" "$appdir/usr/share/icons/"
done
cp "$root_dir/packaging/appimage/AppRun" "$appdir/AppRun"
chmod +x "$appdir/AppRun"
cp "$root_dir/packaging/appimage/arboretum.desktop" "$appdir/arboretum.desktop"
cp "$root_dir/arboretum-icon.png" "$appdir/arboretum.png"
arboretum_write_build_info "$appdir/usr/share/arboretum/BUILD-INFO.txt"

# Fixed upstream releases and hashes: never silently switch build tools.
curl -fL --retry 3 -o "$work_dir/appimagetool.AppImage" \
  https://github.com/AppImage/appimagetool/releases/download/1.9.1/appimagetool-x86_64.AppImage
curl -fL --retry 3 -o "$work_dir/runtime-x86_64" \
  https://github.com/AppImage/type2-runtime/releases/download/20251108/runtime-x86_64
printf '%s  %s\n' \
  ed4ce84f0d9caff66f50bcca6ff6f35aae54ce8135408b3fa33abfc3cb384eb0 "$work_dir/appimagetool.AppImage" \
  2fca8b443c92510f1483a883f60061ad09b46b978b2631c807cd873a47ec260d "$work_dir/runtime-x86_64" | sha256sum -c -
chmod +x "$work_dir/appimagetool.AppImage"
ARCH=x86_64 APPIMAGE_EXTRACT_AND_RUN=1 "$work_dir/appimagetool.AppImage" \
  --runtime-file "$work_dir/runtime-x86_64" "$appdir" "$root_dir/dist/Arboretum-Linux-x64.AppImage"
