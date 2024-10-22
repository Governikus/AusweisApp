#!/bin/sh

if [ -z "$BUILD_URL" ]; then
echo "Warning: This script should be called on jenkins only"
  exit 1
fi

if [ -n "$1" ]; then
  PATCH=$1
else
  PATCH="../patch.diff"
fi

REVISION_CURRENT=$(hg id -i)

# Check if the current repository state is formatted
cmake --build ../build --target format
STATUS_CURRENT=$(hg status | wc -c)
if [ "$STATUS_CURRENT" != "0" ]; then
  echo 'Current repository state is not formatted!'
  hg addremove
  hg diff
  hg commit -m "fix formatting" -s
fi
REVISION_FORMATTED=$(hg id -i)

if [ ! -f "$PATCH" ] && [ -n "$NO_PATCH" ]; then
  if [ "$STATUS_CURRENT" != "0" ]; then
      echo 'FORMATTING FAILED'
  else
      echo 'FORMATTING SUCCEDED'
  fi
  exit 0
fi

# Rollback any changes
hg update -C -r "$REVISION_CURRENT"

# Apply patch on the current repository state
hg --config patch.eol=auto --config phases.new-commit=secret import -m 'jenkins patch formatting' -d 'today' -u 'jenkins' "$PATCH"
if [ "$?" != "0" ]; then
  echo 'FORMATTING FAILED: Patch cannot be applied'
  exit 0
fi

# Check if the repository state is formatted after the patch
cmake --build ../build --target rebuild_cache
cmake --build ../build --target format
STATUS_PATCH=$(hg status | wc -c)
if [ "$STATUS_PATCH" = "0" ]; then
  if [ "$STATUS_CURRENT" != "0" ]; then
    echo 'Patch fixes repository formatting.'
  fi
  echo 'FORMATTING SUCCEDED'
  exit 0
elif [ "$STATUS_CURRENT" = "0" ]; then
  echo 'FORMATTING FAILED: Patch is not formatted'
  hg diff
  hg revert -a -C
  exit 0
fi

# Checkout formatted repository state
hg update -C -r "$REVISION_FORMATTED"

# Apply patch on the formatted repository state
hg --config patch.eol=auto --config phases.new-commit=secret import -m 'jenkins patch formatting' -d 'today' -u 'jenkins' "$PATCH"
if [ "$?" != "0" ]; then
  echo 'FORMATTING FAILED: Patch cannot be applied, because the current repository state is unformatted and the patch conflicts with the formatted repository state without fixing the formatting!'
  exit 0
fi

# Check if the patch itself is formatted even though is does not fix
# the unformatted repository state
cmake --build ../build --target rebuild_cache
cmake --build ../build --target format
STATUS_PATCH=$(hg status | wc -c)
if [ "$STATUS_PATCH" != "0" ]; then
  echo 'FORMATTING FAILED: Patch is not formatted'
  hg diff
  hg revert -a -C
  exit 0
fi

echo 'FORMATTING SUCCEDED: Patch does not introduce new formatting issues.'
