FROM alpine:3.22

ARG JENKINS_SWARM_VERSION=3.48
ENV EXECUTOR=3 LABELS= NAME= PASSWORD=

RUN adduser governikus -s /bin/sh -D
RUN apk --no-cache add openjdk17-jre-headless cmake samurai make tini gnupg mercurial py3-pip wget rbtools uv && \
    pip3 install --break-system-packages python-hglib
RUN wget -O /swarm-client.jar https://repo.jenkins-ci.org/releases/org/jenkins-ci/plugins/swarm-client/$JENKINS_SWARM_VERSION/swarm-client-$JENKINS_SWARM_VERSION.jar

ADD swarm.sh /
