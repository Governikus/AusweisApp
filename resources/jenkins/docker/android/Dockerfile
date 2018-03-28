FROM arch:latest
MAINTAINER Governikus KG <ausweisapp2@governikus.com>

ENV NAME=Android LABELS=Android

RUN echo "[multilib]" >> /etc/pacman.conf && echo 'Include = /etc/pacman.d/mirrorlist' >> /etc/pacman.conf

RUN chown -R governikus: /var/cache/pacman/pkg/

ARG JENKINS_SWARM_VERSION=3.8
ARG TINI_VERSION=0.16.1
RUN curl -L -o /sbin/tini https://github.com/krallin/tini/releases/download/v$TINI_VERSION/tini-static-muslc-amd64 && chmod 755 /sbin/tini && \
    curl -L -o /swarm-client.jar https://repo.jenkins-ci.org/releases/org/jenkins-ci/plugins/swarm-client/$JENKINS_SWARM_VERSION/swarm-client-$JENKINS_SWARM_VERSION.jar
ADD ../swarm/swarm.sh /

USER governikus
RUN mkdir -p /home/governikus/.ccache && mkdir -p /home/governikus/workspace && mkdir -p /home/governikus/packages
VOLUME /home/governikus/.ccache

# key for ncurses sources
RUN gpg --receive-keys 702353E0F7E48EDB

RUN pacaur -Sy --noconfirm cmake ccache python2-hglib apache-ant jdk8-openjdk jre8-openjdk-headless mercurial python2-hglib \
                          android-ndk-10e android-sdk-25.2.5 android-sdk-build-tools android-sdk-platform-tools \
                          android-platform-18 android-platform-21 android-platform-25 \
                          && rm -rf /var/cache/pacman/pkg/* && rm -rf /home/governikus/.cache/pacaur

ENTRYPOINT ["/sbin/tini", "--"]
CMD sh -l -c /swarm.sh
