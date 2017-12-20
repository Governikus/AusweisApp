#!/bin/sh
set -e

. "$(dirname "$0")"/common.sh

for arg in "$@"
do
  case "$arg" in
    format) ENABLE_FORMAT=true;;
    docs)   ENABLE_DOCS=true;;
    *)      echo "Unknown command: $1" && exit 1
  esac
done

alpine_run prepare <<-EOF
  mkdir -p /tmp/build/format
  mkdir -p /tmp/build/docs
EOF



if [ "$ENABLE_FORMAT" = true ]; then
alpine_run format <<-EOF
  cd /tmp/build/format

  cmake -Dtools.only=ON ~
  make format
  cd ~
  git add .
  git --no-pager diff --staged
  git diff-index --quiet HEAD
EOF
fi


if [ "$ENABLE_DOCS" = true ]; then
alpine_run docs <<-EOF
  cd /tmp/build/docs

  cmake -Dtools.only=ON ~
  make sdk
  make notes
EOF
fi
