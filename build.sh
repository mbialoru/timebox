#!/usr/bin/env bash
#
# Copyright 2021 Marek "Saligia" Białoruski
# Project available under MIT License
#
# Project structure based on Konstantin Gredeskoul CMake template:
# https://github.com/kigster/cmake-project-template
#
# WARNING: This BASH script is completely OPTIONAL.
#
# If you choose to run this script to build the project, run:
#
#     $ ./build.sh
#
# It will clean, build and run the tests.

[[ -z $(which git) ]] && {
  echo "You need git installed."
  exit 1
}

export BashMatic="${HOME}/.bashmatic"

[[ -s ${BashMatic}/init.sh ]] || {
  rm -rf "${BashMatic}" 2>/dev/null
  git clone https://github.com/kigster/bashmatic "${BashMatic}" 2>&1 1>/dev/null
}

source "${BashMatic}/init.sh"

export ProjectRoot=$(pwd)
export BuildDir="${ProjectRoot}/build"
export BashLibRoot="${ProjectRoot}/bin/lib-bash"
export LibBashRepo="https://github.com/kigster/lib-bash"
export ProjectName="divider"

project.header() {
  h1.purple "<INSERT ${ProjectName} DESCRIPTION HERE>"
  local OIFC=${IFC}
  IFS="|" read -r -a gcc_info <<< "$(gcc --version 2>&1 | tr '\n' '|')"
  export IFC=${OIFC}
  h1 "${bldylw}GCC" "${gcc_info[1]}" "${gcc_info[2]}" "${gcc_info[3]}" "${gcc_info[4]}"
  h1 "${bldylw}GIT:    ${bldblu}$(git --version)"
  h1 "${bldylw}CMAKE:  ${bldblu}$(cmake --version | tr '\n' ' ')"
}

project.setup() {
  hl.subtle "Creating Build Folder..."
  run "mkdir -p build"
  run "git submodule init && git submodule update"
}

project.clean() {
  hl.subtle "Cleaning output folders..."
  run 'rm -rf bin/d* include/d* lib/*'
}

project.build() {
  run "cd build"
  run.set-next show-output-on
  run "cmake .. -DCMAKE_BUILD_TYPE=Release && cmake --build . --parallel 2 --config Release --target install"
  run "cd ${ProjectRoot}"
}

project.tests() {
  if [[ -f bin/${ProjectName}_tests ]]; then
    run.set-next show-output-on
    run "echo && bin/${ProjectName}_tests"
  else
    printf "${bldred}Can't find installed executable ${bldylw}bin/${ProjectName}_tests.${clr}\n"
    exit 2
  fi
}

main() {
  project.header
  project.setup
  project.build
  project.tests
}

(( $_s_ )) || main
