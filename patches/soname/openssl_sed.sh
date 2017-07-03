#!/bin/sh
echo "Patching Configure..."
#sed -i 's/.so.\\$(SHLIB_MAJOR).\\$(SHLIB_MINOR)/.so/g' Configure
sed -i 's/.\\$(SHLIB_MAJOR).\\$(SHLIB_MINOR)//g' Configure

echo "Patching Makefile..."
sed -i 's/$$SHLIB$$SHLIB_SOVER$$SHLIB_SUFFIX/$$SHLIB/g' Makefile.shared

