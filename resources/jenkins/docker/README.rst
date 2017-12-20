Jenkins Swarm
=============

Diese Dockerfiles werden zum Generieren von Jenkins Agents
verwendet. Das Shellskript "generate.sh" erstellt alle
Docker Images anhand der Dockerfiles neu.

Alle Basis-Images werden "FROM scratch" erzeugt. Dafür
ist es notwendig das "rootfs" manuell in die folgenden
Ordner zu kopieren.

Alpine
------
 - https://alpinelinux.org/downloads/
 - alpine-minirootfs-*-x86_64.tar.gz

Arch
----
 - https://mirrors.kernel.org/archlinux/iso/latest/
 - archlinux-bootstrap-*-x86_64.tar.gz

 - https://busybox.net/downloads/binaries/
 - busybox-x86_64

