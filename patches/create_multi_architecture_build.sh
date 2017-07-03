#!/bin/bash

DIR=$1
DIR64=$2
PREFIX_PATH=$3

#
# Copy content of all folders except the library files
#
echo "Copy content to ${PREFIX_PATH}:"
for dir in ${DIR}/* ; do
    if [ -d $dir ]
    then
	dirname=$(basename "$dir")
        if [ "$dirname" != "lib" ]
        then
            echo "* Copy content of ${dir}"
            mkdir -p ${PREFIX_PATH}/${dirname}
            cp -R ${dir}/* ${PREFIX_PATH}/${dirname}
        fi
    fi
done

#
# Make multi-architecture libraries
#
echo "Create multi-architecture libraries in ${PREFIX_PATH}/lib:"
mkdir -p ${PREFIX_PATH}/lib
cd ${PREFIX_PATH}/lib
for file in ${DIR}/lib/* ; do
    filename=$(basename "$file")

    if [[ ( $filename == *.dylib ) || ( $filename == *.a ) ]]
    then
	if [ -h $file ]
	then
	    # create symbolic links for multi-architecture library

	    resolvedfilename=$(readlink "$file")
	    echo "* Create sym link ${filename}"
	    ln -s ${resolvedfilename} ${filename}
	else
	    # create multi-architecture library

	    file64=${DIR64}/lib/${filename}
	    if [ -f $file64 ]
	    then
		echo "* Create lib ${filename}"
		lipo $file $file64 -create -output ${filename}
	    else
		echo "* 64bit library not found ${file64}"
	    fi
	fi
    elif [[ ( $filename == pkgconfig ) ]]
    then

	mkdir -p ${PREFIX_PATH}/lib/pkgconfig
	for configfile in ${DIR}/lib/pkgconfig/* ; do
	    configfilename=$(basename "$configfile")
	    sed "s:${DIR}:${PREFIX_PATH}:g" "$configfile" > pkgconfig/${configfilename}
	    echo "* Copy adapted package config file ${configfilename}"
	done
    elif [ -f $file ]
    then
        echo "* Skip file ${filename}"
    else
        echo "* Skip directory ${filename}"
    fi
done

