readonly ALPINE_ROOT='/mnt/alpine'
readonly ALPINE_USER='governikus'
readonly ALPINE_HOME="${ALPINE_ROOT}/home/${ALPINE_USER}"
readonly ALPINE_SRC="${ALPINE_HOME}/src"
readonly ALPINE_CCACHE="${ALPINE_HOME}/.ccache"

alpine_run() {
  local folding="${1:-chroot}"
  local user="${2:-${ALPINE_USER}}"
  local cmd="${3:-sh}"

  echo -e "travis_fold:start:$folding"
  chroot "$ALPINE_ROOT" /usr/bin/env -i su -l $user sh -c "cd; $cmd"
  echo -e "travis_fold:end:$folding"
}
