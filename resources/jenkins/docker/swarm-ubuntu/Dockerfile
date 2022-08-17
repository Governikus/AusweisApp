FROM ubuntu:22.04
MAINTAINER Governikus KG <support@ausweisapp.de>

ARG JENKINS_SWARM_VERSION=3.29
ENV EXECUTOR=3 LABELS= NAME= PASSWORD=
ENV DEBIAN_FRONTEND noninteractive

RUN useradd -m governikus
RUN apt-get update && \
    apt-get -y install openjdk-11-jre-headless tini python3-pip wget && \
    pip3 install rbtools mercurial python-hglib && \
    rm -rf /var/lib/apt/lists/*
RUN wget -O /swarm-client.jar https://repo.jenkins-ci.org/releases/org/jenkins-ci/plugins/swarm-client/$JENKINS_SWARM_VERSION/swarm-client-$JENKINS_SWARM_VERSION.jar

ADD swarm.sh /
