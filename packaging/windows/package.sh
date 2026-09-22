#!/usr/bin/env bash
set -euo pipefail

root_dir=$(cd "$(dirname "$0")/../.." && pwd)
output_dir="$root_dir/dist/Arboretum-Windows-x64"
source "$root_dir/packaging/version.sh"

rm -rf "$output_dir"
mkdir -p "$output_dir/share"

gcc -O2 -g -Wall -Wextra -Wno-deprecated-declarations -Wno-unused-parameter \
  -DARBORETUM_BUILD_ID="\"$build_id\"" \
  $(pkg-config --cflags gtk4) "$root_dir/baumg.c" -o "$output_dir/arboretum.exe" \
  $(pkg-config --libs gtk4) -lm

# GTK and its dependencies are distributed as DLLs.  Follow every dependency
# that comes from the UCRT64 runtime and place it next to the executable.
declare -A copied=()
queue=("$output_dir/arboretum.exe")
while ((${#queue[@]})); do
  file=${queue[0]}
  queue=("${queue[@]:1}")
  while IFS= read -r library; do
    name=$(basename "$library")
    [[ ${copied[$name]+present} ]] && continue
    copied[$name]=1
    cp -L "$library" "$output_dir/$name"
    queue+=("$output_dir/$name")
  done < <(ldd "$file" | awk '$3 ~ /^\/ucrt64\/bin\// {print $3}')
done

# Runtime data used by GTK is needed in addition to the DLLs.
cp -a /ucrt64/share/glib-2.0 "$output_dir/share/"
cp -a /ucrt64/share/gtk-4.0 "$output_dir/share/"
cp -a /ucrt64/share/icons "$output_dir/share/"
cp -a "$root_dir/arboretum-icon.png" "$output_dir/arboretum-icon.png"
cp "$root_dir/packaging/windows/Diagnose.bat" "$output_dir/Diagnose.bat"
cp "$root_dir/packaging/windows/Dateitest.bat" "$output_dir/Dateitest.bat"
arboretum_write_build_info "$output_dir/BUILD-INFO.txt"

cat > "$output_dir/Arboretum.bat" <<'EOF'
@echo off
setlocal
set "ROOT=%~dp0"
set "PATH=%ROOT%;%PATH%"
set "XDG_DATA_DIRS=%ROOT%share"
set "GSETTINGS_SCHEMA_DIR=%ROOT%share\glib-2.0\schemas"
start "" "%ROOT%arboretum.exe" %*
EOF

cd "$root_dir/dist"
rm -f Arboretum-Windows-x64.zip
zip -qr Arboretum-Windows-x64.zip Arboretum-Windows-x64
