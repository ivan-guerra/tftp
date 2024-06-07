#!/bin/bash

# Launches a container with the toolchain required to build the tftp server
# and client. You still need to run the CMake configure and build commands 
# within the container.

if [ -z "$CCACHE_DIR" ]; then
  ccache_dir="/home/${USER}/.cache/ccache"
  echo "warning: CCACHE_DIR not set, assuming '${ccache_dir}'"
else
  ccache_dir=$CCACHE_DIR
fi

# This abomination gets the project source/root directory.
source_dir="$(dirname "$(cd "$(dirname "${BASH_SOURCE[1]}")" && pwd)")"

# Couple of notes about this docker run command:
#   * CC/CXX are explicitly set so that the clang C/C++ compilers use CCACHE.
#   * The -u ... argument ensures that the container user has the UID and GID 
#     the of the host user. This makes it so that any build artefacts produced 
#     in the container are owned by the host user.
#   * The host's ccache dir and the tftp source directory are mounted as volumes
#     within the container. This allows builds to leverage the cache on the 
#     host. It also ensures build artefacts persist between container runs.
docker run --rm -it \
    -e CCACHE_DIR="/ccache" \
    -e CC="/usr/lib64/ccache/clang" \
    -e CXX="/usr/lib64/ccache/clang++" \
    -u "$(id -u "$USER")":"$(id -g "$USER")" \
    -v "$ccache_dir":/ccache:rw \
    -v "$source_dir":"$source_dir" \
    tftp-toolchain:latest
