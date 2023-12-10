#!/bin/bash

CWD=$(pwd)

# Root directory.
TFTP_PROJECT_PATH=$(dirname ${CWD})

# Scripts directory.
TFTP_SCRIPTS_PATH="${TFTP_PROJECT_PATH}/scripts/linux"

# Binary directory.
TFTP_BIN_DIR="${TFTP_PROJECT_PATH}/bin"

# Resource directory.
TFTP_RES_DIR="${TFTP_PROJECT_PATH}/resources"

# CMake build files and cache.
TFTP_BUILD_DIR="${TFTP_PROJECT_PATH}/build"
