#!/bin/sh
set -e

. "$(dirname "$0")"/common.sh

for arg in "$CHECK"
do
  case "$arg" in
    format) ENABLE_FORMAT=true;;
    docs)   ENABLE_DOCS=true;;
    build)  ENABLE_BUILD=true;;
    *)      echo "Unknown command: $1" && exit 1
  esac
done

alpine_run prepare <<-EOF
  mkdir ~/format
  mkdir ~/docs
  mkdir ~/app
EOF



if [ "$ENABLE_FORMAT" = true ]; then
alpine_run format <<-EOF
  set -e
  cd ~/format

  cmake -Dtools.only=ON ~/src
  make format
  cd ~/src
  git add .
  git --no-pager diff --staged
  git diff-index --quiet HEAD
EOF
fi


if [ "$ENABLE_DOCS" = true ]; then
alpine_run docs <<-EOF
  set -e
  cd ~/docs

  cmake -Dtools.only=ON ~/src
  make sdk
  make notes
EOF
fi


if [ "$ENABLE_BUILD" = true ]; then
alpine_run build <<-EOF
  set -e
  cd ~/app

  cmake -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER=$COMPILER -DCMAKE_BUILD_TYPE=$TYPE ~/src
  cmake --build .
  ctest --output-on-failure
  src/AusweisApp2 -platform offscreen --help
EOF
fi

