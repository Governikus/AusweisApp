#!/bin/sh
set -eu

. "$(dirname "$0")"/common.sh

readonly CLONE_DIR="${CLONE_DIR:-$(pwd)}"

MINIROOTFS_VERSION="3.10.0"
MINIROOTFS_SHA="ec3da7fb5f709a1ce912e6e31fccc5588420c5f1dcecc362c72c989532c1917a"

MINIROOTFS="alpine-minirootfs-${MINIROOTFS_VERSION}-x86_64.tar.gz"
MINIROOTFS_URI="http://dl-cdn.alpinelinux.org/alpine/v${MINIROOTFS_VERSION%.*}/releases/x86_64/$MINIROOTFS"

cd /tmp
wget "$MINIROOTFS_URI"
echo "$MINIROOTFS_SHA  $MINIROOTFS" | sha256sum -c

mkdir -p "${ALPINE_ROOT}"
tar xf "$MINIROOTFS" -C "${ALPINE_ROOT}"

chmod 755 "$ALPINE_ROOT"
cd "$ALPINE_ROOT"

cp /etc/resolv.conf etc/resolv.conf
mount -t proc none proc
mount --rbind /sys sys
mount --rbind /dev dev
mount --rbind /run run
ln -sf /run/shm dev/shm


PACKAGES="cmake make ninja git"
for arg in "$CHECK"
do
  case "$arg" in
    format) PACKAGES="$PACKAGES uncrustify";;
    docs)   PACKAGES="$PACKAGES py2-sphinx py2-setuptools";;
    build)  PACKAGES="$PACKAGES clang g++ ccache binutils-gold eudev-dev pcsc-lite-dev http-parser-dev openssl-dev qt5-qtsvg-dev qt5-qtquickcontrols2-dev qt5-qtwebsockets-dev qt5-qtconnectivity-dev qt5-qtbase-dev qt5-qttools-dev";;
    *)      echo "Unknown command: $1" && exit 1
  esac
done


alpine_run packages root <<-EOF
  adduser $ALPINE_USER -G users -s /bin/sh -D
  apk upgrade -U -a
  apk add $PACKAGES
EOF

mkdir ${ALPINE_SRC}
mkdir ${ALPINE_CCACHE}
mount --bind "$CLONE_DIR" "${ALPINE_SRC}"
mount --bind "$HOME/.ccache" "${ALPINE_CCACHE}"
chmod 777 "$ALPINE_CCACHE"

alpine_run chown root <<-EOF
  chown -R $ALPINE_USER: /home/${ALPINE_USER}
EOF
