FROM alpine:swarm
MAINTAINER Governikus KG <ausweisapp2@governikus.com>

ENV NAME=Common LABELS=Common

RUN apk --no-cache add cmake make uncrustify py2-pip && pip install python-hglib && apk del py2-pip

USER governikus

ENTRYPOINT ["/sbin/tini", "--"]
CMD /swarm.sh
