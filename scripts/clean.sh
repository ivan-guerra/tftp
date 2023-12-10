#!/bin/bash

source config.sh

# Remove the binary directory.
if [ -d $TFTP_BIN_DIR ]
then
    echo "removing '$TFTP_BIN_DIR'"
    rm -r $TFTP_BIN_DIR
fi

# Remove the CMake build directory.
if [ -d $TFTP_BUILD_DIR ]
then
    echo "removing '$TFTP_BUILD_DIR'"
    rm -rf $TFTP_BUILD_DIR
fi
