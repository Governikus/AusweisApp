FROM alpine:3.19
MAINTAINER Governikus KG <support@ausweisapp.de>

ARG JENKINS_SWARM_VERSION=3.44
ENV EXECUTOR=3 LABELS= NAME= PASSWORD=

RUN adduser governikus -s /bin/sh -D
RUN apk --no-cache add openjdk11-jre-headless cmake samurai make tini mercurial py3-pip py3-hglib wget rbtools
RUN wget -O /swarm-client.jar https://repo.jenkins-ci.org/releases/org/jenkins-ci/plugins/swarm-client/$JENKINS_SWARM_VERSION/swarm-client-$JENKINS_SWARM_VERSION.jar

ADD swarm.sh /
