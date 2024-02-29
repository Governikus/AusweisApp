FROM ubuntu:23.04
MAINTAINER Governikus KG <support@ausweisapp.de>

ARG JENKINS_SWARM_VERSION=3.44
ENV EXECUTOR=3 LABELS= NAME= PASSWORD=
ENV DEBIAN_FRONTEND noninteractive
ENV PIP_BREAK_SYSTEM_PACKAGES=1

# Use optional remote ccache
# redis://YOUR_SERVER:6379
ENV CCACHE_REMOTE_STORAGE= CCACHE_RESHARE=true CCACHE_REMOTE_ONLY=

RUN useradd -m governikus -g users -u 1111
RUN apt-get update && \
    apt-get -y install openjdk-17-jre-headless tini python3-pip wget && \
    pip3 install rbtools mercurial python-hglib && \
    rm -rf /var/lib/apt/lists/*
RUN wget -O /swarm-client.jar https://repo.jenkins-ci.org/releases/org/jenkins-ci/plugins/swarm-client/$JENKINS_SWARM_VERSION/swarm-client-$JENKINS_SWARM_VERSION.jar

ADD swarm.sh /
