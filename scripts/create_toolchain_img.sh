#!/bin/bash

# Construct a docker image containing the tftp toolchain. See build.sh for
# the appropriate container run command.

project_root="$(dirname "$(cd "$(dirname "${BASH_SOURCE[1]}")" && pwd)")"
docker_file_path="${project_root}/docker"

docker build "$docker_file_path" \
  --build-arg="SRC_DIR=$project_root" \
  -t tftp-toolchain:latest
