#!/bin/bash

# Updates all icons in the application from github.com/KDE/breeze-icons

repo="KDE/breeze-icons"

# Get current version
current=$(cat version)
if [ "$?" -ne 0 ]; then
  echo "Version not found"
  return 1
fi

# Get latest version
latest=$(curl -Ss https://api.github.com/repos/$repo/tags | jq -r '.[0].name')
if [ "$latest" = "$current" ]; then
  echo "Already up-to-date"
  return 0
fi

# Notify we have to update
echo "Latest version is $latest, you have $current"

# Clone icons
repo_dir="/tmp/$(basename $repo)-$latest"
git clone --depth 1 --branch "$latest" https://github.com/$repo.git "$repo_dir" &>/dev/null

# git fails if directory already exists, use it instead
if [ ! -d "$repo_dir" ]; then
  echo "Download failed"
  return 1
fi

# Icons in main toolbar should be 22x22
# Otherwise, they should be 16x16
large_icons=(
  application-menu
  edit-find
  media-skip-backward
  media-playback-start
  media-playback-pause
  media-skip-forward
  media-playlist-shuffle
  media-playlist-repeat
  window-minimize-symbolic
  window-close-dymbolic)

if [[ "$large_icons" =~ " application-name " ]]; then
  echo "found"
fi
