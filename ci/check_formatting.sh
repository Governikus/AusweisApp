#!/bin/sh

set -e

if [ -z "$CI" ]; then
  echo "Warning: This script should be called on CI only"
  exit 1
fi

PATCH=$1

REVISION_CURRENT=$(hg id -i)
echo "REVISION_CURRENT: ${REVISION_CURRENT}"

format_repository() {
  cmake --build ../build --target format
  cmake --build ../build --target update.translations
}

# Check if the current repository state is formatted
format_repository
STATUS_CURRENT=$(hg status | wc -c)
if [ ! "0" = "$STATUS_CURRENT" ]; then
  echo 'Current repository state is not formatted!'
  hg addremove
  hg diff
  hg commit -m "fix formatting" -s -u 'CI'
fi
REVISION_FORMATTED=$(hg id -i)
echo "REVISION_FORMATTED: ${REVISION_FORMATTED}"

if [ ! -f "$PATCH" ] && [ -n "$NO_PATCH" ]; then
  if [ ! "0" = "$STATUS_CURRENT" ]; then
      echo 'FORMATTING FAILED'
  else
      echo 'FORMATTING SUCCEEDED'
  fi
  exit 0
fi

# Rollback any changes
echo "Rollback to ${REVISION_CURRENT}"
hg update -C -r "$REVISION_CURRENT"

# Apply patch on the current repository state
if ! hg --config patch.eol=auto --config phases.new-commit=secret import -m 'jenkins patch formatting' -d 'today' -u 'CI' "$PATCH"; then
  echo 'FORMATTING FAILED: Patch cannot be applied'
  exit 0
fi

# Check if the repository state is formatted after the patch
cmake --build ../build --target rebuild_cache
format_repository
STATUS_PATCH=$(hg status | wc -c)
if [ "$STATUS_PATCH" = "0" ]; then
  if [ ! "0" = "$STATUS_CURRENT" ]; then
    echo 'Patch fixes repository formatting.'
  fi
  echo 'FORMATTING SUCCEEDED'
  exit 0
elif [ ! "0" = "$STATUS_CURRENT" ]; then
  echo 'FORMATTING FAILED: Patch is not formatted'
  hg diff
  hg revert -a -C
  exit 0
fi

# Checkout formatted repository state
hg update -C -r "$REVISION_FORMATTED"

# Apply patch on the formatted repository state
if ! hg --config patch.eol=auto --config phases.new-commit=secret import -m 'jenkins patch formatting' -d 'today' -u 'CI' "$PATCH"; then
  echo 'FORMATTING FAILED: Patch cannot be applied, because the current repository state is unformatted and the patch conflicts with the formatted repository state without fixing the formatting!'
  exit 0
fi

# Check if the patch itself is formatted even though is does not fix
# the unformatted repository state
cmake --build ../build --target rebuild_cache
format_repository
STATUS_PATCH=$(hg status | wc -c)
if [ ! "0" = "$STATUS_PATCH" ]; then
  echo 'FORMATTING FAILED: Patch is not formatted'
  hg diff
  hg revert -a -C
  exit 0
fi

echo 'FORMATTING SUCCEEDED: Patch does not introduce new formatting issues.'
