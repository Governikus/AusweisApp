#!/bin/sh

dest=${DESTINATION:-/home/governikus}
name=${NAME:-undefined}
executor=${EXECUTOR:-1}
labels=${LABELS:-undefined}
mode=${MODE:-exclusive}
master=${MASTER:-https://ausweisapp-ci.govkg.de/}
user=${USER:-jenkins-swarm}
fingerprints=${FINGERPRINTS:-63:42:C6:90:7F:7A:99:8E:B7:6B:2D:1D:FC:B4:31:2B:11:83:6D:87:8E:4A:8F:F6:0D:0C:07:6D:77:2E:A0:E6}

/usr/bin/java                                          \
    -jar /swarm-client.jar                             \
    -name $name-$HOSTNAME                              \
    -mode $mode                                        \
    -executors $executor                               \
    -labels "$labels"                                  \
    -master $master                                    \
    -username $user                                    \
    -passwordEnvVariable PASSWORD                      \
    -sslFingerprints $fingerprints                     \
    -fsroot $dest                                      \
    -disableClientsUniqueId

