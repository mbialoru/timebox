# sid(unstable), bookworm(test), bullseye(11), buster(10)
# available in -slim variants
ARG VARIANT="sid-slim"
FROM debian:${VARIANT}

ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID
ARG MIRROR=poland

# Set root password
RUN echo 'root:docker' | chpasswd

# Update and upgrade
RUN apt update && apt -y upgrade

# Set fastest available mirror
RUN apt install -y netselect-apt
RUN netselect-apt -c ${MIRROR} -t 15 -a amd64 -n unstable
RUN mv sources.list /etc/apt/sources.list
RUN apt -y update

# Install necessary packages available from standard repos
RUN apt install -y apt-utils wget file zip software-properties-common

# Install needed tools, packages and libraries
RUN apt install -y build-essential binutils autoconf automake cmake \
    cmake-curses-gui meson ninja-build git ruby graphviz doxygen moreutils \
    cppcheck ccache gdb

# Install Python
RUN apt install -y python2 python3 python3-pip python3-venv

# Python packages
RUN pip install --upgrade pip setuptools autopep8 conan cmakelang
RUN conan --version

# Install GCC
ARG GCC_VER="12"
RUN apt install -y gcc-${GCC_VER} g++-${GCC_VER}

# Set gcc-${GCC_VER} as default gcc
RUN update-alternatives --install /usr/bin/gcc gcc $(which gcc-${GCC_VER}) 100
RUN update-alternatives --install /usr/bin/g++ g++ $(which g++-${GCC_VER}) 100

# Install clang llvm
ARG LLVM_VER="15"
RUN apt install -y clang-${LLVM_VER} lldb-${LLVM_VER} lld-${LLVM_VER} \
    clangd-${LLVM_VER} llvm-${LLVM_VER}-dev libclang-${LLVM_VER}-dev \
    clang-tidy-${LLVM_VER} clang-format-${LLVM_VER}

# Set the default clang-tidy, so CMake can find it
RUN update-alternatives --install /usr/bin/clang-tidy clang-tidy \
    $(which clang-tidy-${LLVM_VER}) 1

# Set default clang-format
RUN update-alternatives --install /usr/bin/clang-format clang-format \
    $(which clang-format-${LLVM_VER}) 1

# Set clang-${LLVM_VER} as default clang
RUN update-alternatives --install /usr/bin/clang clang $(which \
    clang-${LLVM_VER}) 100
RUN update-alternatives --install /usr/bin/clang++ clang++ $(which \
    clang++-${LLVM_VER}) 100

# Install this project dependencies
RUN apt install -y libboost-all-dev libgtest-dev libgmock-dev libbenchmark-dev \
    libgl1-mesa-dev freeglut3-dev libxext-dev pkg-config

# Symbolic link for compatibility
# RUN ln -s /bin/uname /usr/bin/uname

# Cleanup cached apt data we don't need anymore
RUN apt autoremove -y && apt clean && rm -rf /var/lib/apt/lists/*

# Create the user
RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME -s /bin/bash
USER $USERNAME

# Add alias for python3
RUN echo "alias python=python3" >> ~/.bashrc

# Change default cd directory to app
RUN echo "alias cd='HOME=/workspaces/timebox cd'" >> ~/.bashrc

# Add convenient aliases
RUN echo "alias l='ls -a'" >> ~/.bashrc
RUN echo "alias ll='ls -alFh'" >> ~/.bashrc