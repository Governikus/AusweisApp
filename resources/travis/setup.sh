#!/bin/sh
set -eu

. "$(dirname "$0")"/common.sh

readonly CLONE_DIR="${CLONE_DIR:-$(pwd)}"

MINIROOTFS_VERSION="3.6.2"
MINIROOTFS_SHA="df4bf81fdafdc72b32ad455c23901935fdfe5815993612ba7a2df4bae79d97ca"

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
for arg in "$@"
do
  case "$arg" in
    format) PACKAGES="$PACKAGES uncrustify";;
    docs)   PACKAGES="$PACKAGES py2-sphinx py2-setuptools";;
    *)      echo "Unknown command: $1" && exit 1
  esac
done


alpine_run packages root <<-EOF
  adduser $ALPINE_USER -G users -s /bin/sh -D
  apk upgrade -U -a
  apk add $PACKAGES
EOF

mount --bind "$CLONE_DIR" "${ALPINE_ROOT}/home/${ALPINE_USER}"

alpine_run chown root <<-EOF
  chown -R $ALPINE_USER: /home/${ALPINE_USER}
EOF
