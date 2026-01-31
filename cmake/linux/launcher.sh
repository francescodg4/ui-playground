#!/bin/sh
# Starts the application with the libraries bundled next to it (the Qt plugins are found through
# the qt.conf beside the executable).
here=$(dirname "$(readlink -f "$0")")
root=$(dirname "$here")
name=$(basename "$0")
export LD_LIBRARY_PATH="$root/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
exec "$root/libexec/$name" "$@"
