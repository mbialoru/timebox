FROM debian:sid-slim

ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID
ARG MIRROR=poland

# Set root password
RUN echo 'root:docker' | chpasswd

# Update and upgrade
RUN apt -y update && apt-get install -y

# Set fastest available mirror
RUN apt -y install netselect-apt
RUN netselect-apt -c ${MIRROR} -t 15 -a amd64 -n unstable
RUN mv sources.list /etc/apt/sources.list
RUN apt -y update

# Install system packages
RUN apt -y install apt-utils

# Install needed tools, packages and libraries
RUN apt install -y build-essential binutils autoconf automake clang clang-12  \
    clang-14 clang-15 gcc-10 g++-10 gcc-12 g++-12 cmake meson ninja-build gdb \
    git ruby graphviz doxygen
RUN apt install -y python2 python3 python3-pip python3-venv
RUN apt install -y libboost-all-dev libgtest-dev libbenchmark-dev \
    libgl1-mesa-dev freeglut3-dev libxext-dev pkg-config

# Python packages
RUN pip install autopep8

# Symbolic link for compatibility if needed
# RUN ln -s /bin/uname /usr/bin/uname

# Create the user
RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME -s /bin/bash
USER $USERNAME