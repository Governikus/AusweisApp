#!/bin/sh
echo "Patching..."

SDK=$1
echo "SDK: ${SDK}"

perl -i -pe "s|^CC= (.*)|CC= \$1 -miphoneos-version-min=7.1|g" Makefile
perl -i -pe "s|^MAKEDEPPROG=makedepend|MAKEDEPPROG= \\\$(CC) -M|g" Makefile
perl -i -pe "s|isysroot\s\S+\s|isysroot ${SDK} |g" Makefile
