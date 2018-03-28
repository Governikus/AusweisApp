#!/bin/sh

dest=${DESTINATION:-/home/governikus}
name=${NAME:-undefined}
executor=${EXECUTOR:-1}
labels=${LABELS:-undefined}
mode=${MODE:-exclusive}
master=${MASTER:-https://vtf-aajenkins.tf.bos-test.de/}
user=${USER:-aajenkins}
fingerprints=${FINGERPRINTS:-68:21:59:5C:C0:B0:1F:78:FF:23:DE:D4:D8:05:0F:74:38:B2:A5:A4:F2:1B:5B:A7:AA:81:2A:A1:46:A8:EB:3A}

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

