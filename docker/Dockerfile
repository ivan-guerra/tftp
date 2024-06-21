# At the time of this writing, Arch Linux does not have official support for 
# clang18. The tftp project uses C++23 features only implemented in clang18+.
# Fedora packages clang18. As a workaround, this Dockerfile builds a Fedora 
# image with all the necessary tools to build the project. If you are on a 
# distro with clang18+, there's no need build this docker image. Otherwise,
# run tftp/scripts/create_toolchain_img.sh to create the toolchain image.
# Afterwards, run build.sh to launch the container from which you can run 
# the CMake configure/build commands.

FROM fedora:latest

# Install all the packages needed to build the tftp client and server.
RUN dnf groupinstall 'Development Tools' -y --nodocs && \
    dnf install -y --nodocs \
        boost-devel \
        cmake \
        clang \
        ccache \
        valgrind \
        libasan \
        libubsan 

# SRC_DIR is the absolute path on the **host** machine to the tftp project
# sources. It should be passed as an arg to the docker build command (see 
# tftp/scripts/create_toolchain_img.sh).
ARG SRC_DIR
RUN mkdir -p ${SRC_DIR}

WORKDIR ${SRC_DIR}
