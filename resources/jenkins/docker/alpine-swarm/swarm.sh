#!/bin/sh

dest=${DESTINATION:-/home/governikus}
name=${NAME:-undefined}
executor=${EXECUTOR:-1}
labels=${LABELS:-undefined}
mode=${MODE:-exclusive}
master=${MASTER:-https://ausweisapp-ci.govkg.de/}
user=${USER:-jenkins-swarm}
fingerprints=${FINGERPRINTS:-FA:B8:35:6B:4F:1F:AF:68:9F:CC:B9:63:71:BC:1F:DA:6E:DB:61:F6:C7:E4:64:F6:41:8F:51:09:4C:A9:A0:A9}

/usr/bin/java                                          \
    -jar /swarm-client.jar                             \
    -webSocket                                         \
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

