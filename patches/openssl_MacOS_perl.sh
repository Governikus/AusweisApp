#!/bin/sh
echo "Patching..."

perl -i -pe "s|^MAKEDEPPROG=makedepend|MAKEDEPPROG= \\\$(CC) -M|g" Makefile
