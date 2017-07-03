#!/bin/sh

BASEDIR=$(dirname $0)
CURRENT_CHANGELOG=$1
LAST_CHANGELOG=$2

if [ "$CURRENT_CHANGELOG" == "" ] || [ "$LAST_CHANGELOG" == "" ]; then
	echo "Usage: CURRENT_CHANGELOG LAST_CHANGELOG"
	exit 1
fi

command -v stat >/dev/null || { echo "stat not found"; exit 1; }

CURRENT_CHANGELOG_SIZE=`stat -c %s $CURRENT_CHANGELOG`
if [ $CURRENT_CHANGELOG_SIZE -lt 66 ]; then
	cp $LAST_CHANGELOG $CURRENT_CHANGELOG
	CURRENT_CHANGELOG=$LAST_CHANGELOG
fi

command -v xsltproc >/dev/null || { echo "xsltproc not found"; exit 1; }

CONTENT=`xsltproc $BASEDIR/changelog.xsl $CURRENT_CHANGELOG 2> /dev/null`

if [ $? != 0 ] || [ "$CONTENT" == "" ]; then
	echo "Erstellung des Changelogs ist fehlgeschlagen!"
	exit 1
fi

echo $CONTENT
