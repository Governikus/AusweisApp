FROM alpine:latest
MAINTAINER Governikus KG <ausweisapp2@governikus.com>

ARG JENKINS_SWARM_VERSION=3.15
ENV EXECUTOR=3 LABELS= NAME= PASSWORD=

RUN adduser governikus -s /bin/sh -D
RUN apk --no-cache add openjdk8-jre tini mercurial py2-hglib wget
RUN wget -O /swarm-client.jar https://repo.jenkins-ci.org/releases/org/jenkins-ci/plugins/swarm-client/$JENKINS_SWARM_VERSION/swarm-client-$JENKINS_SWARM_VERSION.jar

ADD swarm.sh /
