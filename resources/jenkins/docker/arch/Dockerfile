FROM scratch
MAINTAINER Governikus KG <ausweisapp2@governikus.com>

ARG box="busybox-x86_64"
ARG version="2018.01.01"

ADD $box /tmp/busybox
ADD archlinux-bootstrap-$version-x86_64.tar.gz /

# Clean up rootfs
RUN ["/tmp/busybox", "rm", "-rf",      \
     "/root.x86_64/etc/hosts",         \
     "/root.x86_64/etc/hostname",      \
     "/root.x86_64/etc/mtab",          \
     "/root.x86_64/etc/resolv.conf",   \
     "/root.x86_64/dev",               \
     "/root.x86_64/proc",              \
     "/root.x86_64/sys"                \
    ]

RUN ["/tmp/busybox", "sh", "-c", "/tmp/busybox cp -af /root.x86_64/* /"]

RUN rm -rf /tmp/busybox /root.x86_64

# Init Arch
RUN pacman-key --init && pacman-key --populate archlinux

RUN echo 'Server = http://mirrors.kernel.org/archlinux/$repo/os/$arch' > /etc/pacman.d/mirrorlist &&\
    echo 'Server = http://ftp.uni-hannover.de/archlinux/$repo/os/$arch' >> /etc/pacman.d/mirrorlist &&\
    echo 'en_US.UTF-8 UTF-8' > /etc/locale.gen &&\
    echo 'LANG="en_US.UTF-8"' > /etc/locale.conf

RUN pacman -Syu --noconfirm base-devel lzop
RUN locale-gen;

RUN sed -i "s|PKGEXT='.pkg.tar.xz'|PKGEXT='.pkg.tar.lzo'|" /etc/makepkg.conf

RUN useradd governikus -m -s /usr/bin/bash -G wheel
RUN echo '%wheel ALL=(ALL) NOPASSWD: /usr/bin/pacman' > /etc/sudoers.d/pacman

# Helper for AUR
RUN mkdir /tmp/p &&\
    curl -L -o /tmp/p/cower.tar.gz https://aur.archlinux.org/cgit/aur.git/snapshot/cower.tar.gz &&\
    curl -L -o /tmp/p/pacaur.tar.gz https://aur.archlinux.org/cgit/aur.git/snapshot/pacaur.tar.gz

RUN cd /tmp/p && tar xf cower.tar.gz && tar xf pacaur.tar.gz &&\
    chown -R governikus /tmp/p &&\
    su - governikus -c "source /etc/profile.d/perlbin.sh && cd /tmp/p/cower && makepkg -si --noconfirm --skippgpcheck" &&\
    su - governikus -c "source /etc/profile.d/perlbin.sh && cd /tmp/p/pacaur && makepkg -si --noconfirm --skippgpcheck"

# pacaur requires VISUAL, EDITOR or vi
RUN pacman -S --noconfirm vi


RUN rm -rf /tmp/p && rm -rf /var/cache/pacman/pkg/* && rm -rf /home/governikus/.cache/pacaur
