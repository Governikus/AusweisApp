#!/usr/bin/env bash
set -e

################### Alpine
cd alpine
ls alpine-minirootfs-*.tar.gz >/dev/null

echo "Building base Alpine ..."
docker build -t alpine:latest .

images=(swarm trigger common docs linux)
for i in "${images[@]}"
do
	echo "Building $i ..."
	cd ../$i
	docker build -t alpine:$i .
done

