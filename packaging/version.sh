#!/usr/bin/env bash
# Source after root_dir is set. All packages identify the same source commit.
arboretum_version=$(tr -d '\r\n' < "$root_dir/VERSION")
[[ "$arboretum_version" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]] || exit 1
arboretum_commit="${GITHUB_SHA:-}"
if [[ -z "$arboretum_commit" ]]; then
  arboretum_commit=$(git -C "$root_dir" rev-parse HEAD)
fi
[[ "$arboretum_commit" =~ ^[0-9a-f]{40}$ ]] || exit 1
if [[ "${GITHUB_REF_TYPE:-}" == tag && "${GITHUB_REF_NAME:-}" != "v$arboretum_version" ]]; then
  printf 'Tag and VERSION disagree\n' >&2
  exit 1
fi
build_id=${arboretum_commit:0:7}
arboretum_write_build_info() {
  printf 'Arboretum %s\nCommit: %s\nSource: https://github.com/Arbogrammer/Arboretum/tree/%s\nPlatform: %s\nGTK: %s\n' \
    "$arboretum_version" "$arboretum_commit" "$arboretum_commit" "$(uname -sm)" \
    "$(pkg-config --modversion gtk4)" > "$1"
}
