#!/bin/sh

dest=${DESTINATION:-/home/governikus}
name=${NAME:-undefined}
executor=${EXECUTOR:-1}
labels=${LABELS:-undefined}
mode=${MODE:-exclusive}
master=${MASTER:-https://ausweisapp-ci.govkg.de/}
user=${USER:-jenkins-swarm}
fingerprints=${FINGERPRINTS:-74:8C:AE:1C:05:99:95:04:66:CC:B1:27:6F:2B:59:E7:1E:64:47:45:D3:C1:E8:5B:E6:E5:1F:BD:98:97:18:FB}

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

