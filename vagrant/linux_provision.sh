# prevent interactive prompts (i.e. to restart services)
export DEBIAN_FRONTEND=noninteractive

# configure mirror and unstable branch
export MIRROR="poland"
apt-get update && apt-get upgrade -y
apt-get install -y netselect-apt
netselect-apt -c $MIRROR -t 5 -a amd64 -n unstable
mv sources.list /etc/apt/sources.list
apt-get update && apt-get upgrade

# install packages
apt-get install -y apt-utils wget file zip software-properties-common

# install needed tools, packages and libs
apt-get install -y build-essential binutils autoconf automake cmake \
cmake-curses-gui meson ninja-build git ruby graphviz doxygen moreutils \
cppcheck ccache gdb valgrind adjtimex

# install python
apt-get install -y python3 python3-pip python3-venv

# python packages
pip install --upgrade pip setuptools autopep8 conan cmakelang cppclean

# install gcc
export GCC_VER=12
apt-get install -y gcc-$GCC_VER g++-$GCC_VER

update-alternatives --install /usr/bin/gcc gcc $(which gcc-$GCC_VER) 100
update-alternatives --install /usr/bin/g++ g++ $(which g++-$GCC_VER) 100

# install clang
export LLVM_VER=15
apt-get install -y clang-$LLVM_VER lldb-$LLVM_VER lld-$LLVM_VER \
clangd-$LLVM_VER llvm-$LLVM_VER-dev libclang-$LLVM_VER-dev \
clang-tidy-$LLVM_VER clang-format-$LLVM_VER clang-tools-$LLVM_VER

update-alternatives --install /usr/bin/clang-tidy clang-tidy \
$(which clang-tidy-$LLVM_VER) 1
update-alternatives --install /usr/bin/clang-format clang-format \
$(which clang-format-$LLVM_VER) 1
update-alternatives --install /usr/bin/clang clang \
$(which clang-$LLVM_VER) 100
update-alternatives --install /usr/bin/clang++ clang++ \
$(which clang++-$LLVM_VER) 100

# additional tools - not from repository
mkdir /tools && cd /tools
git clone https://github.com/namhyung/uftrace.git && cd uftrace
yes | misc/install-deps.sh
./configure
make && make install

# project dependencies
apt-get install -y libboost-all-dev libgtest-dev libgmock-dev \
libbenchmark-dev libgl1-mesa-dev freeglut3-dev libxext-dev pkg-config \
libserial-dev

# install basic graphical interface
apt-get install -y i3 lightdm slick-greeter stterm

# cleanup cached apt data
apt-get autoremove -y && apt-get clean && rm -rf /var/lib/apt/lists/*

# add aliases
echo "alias python=python3" >> /home/vagrant/.bashrc
echo "alias l='ls -a'" >> /home/vagrant/.bashrc
echo "alias ll='ls -alFh'" >> /home/vagrant/.bashrc

# set default resolution (lightdm with xrandr)
echo "xrandr --output VGA-1 --mode 1600x900" >> /home/vagrant/setres.sh
chmod +x /home/vagrant/setres.sh
sed -i "s@#display-setup-script=@display-setup-script=/home/vagrant/setres.sh@g" /etc/lightdm/lightdm.conf

# provision date tag
date > /etc/vagrant_provisioned_at