#!/bin/sh

dest=${DESTINATION:-/home/governikus}
name=${NAME:-undefined}
executor=${EXECUTOR:-1}
labels=${LABELS:-undefined}
mode=${MODE:-exclusive}
master=${MASTER:-https://ausweisapp-ci.govkg.de/}
user=${USER:-jenkins-swarm}
fingerprints=${FINGERPRINTS:-40:DD:0E:F9:8D:A1:DA:39:D4:B9:B6:31:C3:F8:E5:8B:45:76:C5:BB:68:61:4A:F3:DD:14:16:85:D9:03:C2:02}

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

