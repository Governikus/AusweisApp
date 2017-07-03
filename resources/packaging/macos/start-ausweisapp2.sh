#!/bin/sh

realpath()
{
  [[ $1 = /* ]] && echo "$1" || echo "$PWD/${1#./}"
}

AA2_BASE=`realpath "$0"`
AA2_CONTENTS="${AA2_BASE%/*/*}"
AA2_RESOURCES="$AA2_CONTENTS/Resources"
AA2_PLUGINS="$AA2_CONTENTS/PlugIns"

export QT_LOGGING_CONF="$AA2_RESOURCES/qtlogging.ini"
export QT_MAC_DISABLE_FOREGROUND_APPLICATION_TRANSFORM=true
export QT_QPA_PLATFORM_PLUGIN_PATH=$AA2_PLUGINS

# For debugging what libraries are loaded.
#export DYLD_PRINT_LIBRARIES=1

exec "$AA2_RESOURCES/@EXECUTABLE_NAME@"
