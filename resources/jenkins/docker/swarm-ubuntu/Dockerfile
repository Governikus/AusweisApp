FROM ubuntu:19.04
MAINTAINER Governikus KG <ausweisapp2@governikus.com>

ARG JENKINS_SWARM_VERSION=3.17
ENV EXECUTOR=3 LABELS= NAME= PASSWORD=

RUN useradd -m governikus
RUN apt-get update && \
    apt-get -y install openjdk-8-jre-headless tini mercurial python-hglib wget && \
    rm -rf /var/lib/apt/lists/*
RUN wget -O /swarm-client.jar https://repo.jenkins-ci.org/releases/org/jenkins-ci/plugins/swarm-client/$JENKINS_SWARM_VERSION/swarm-client-$JENKINS_SWARM_VERSION.jar

ADD swarm.sh /
