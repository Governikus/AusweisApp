FROM alpine:swarm
MAINTAINER Governikus KG <ausweisapp2@governikus.com>

ENV NAME=Docs LABELS=Docs

RUN apk --no-cache add cmake make py2-sphinx py2-setuptools py2-pip py2-hglib icu-libs poppler zziplib texlive-full && \
    pip install doc8 cloud_sptheme

USER governikus

ENTRYPOINT ["/sbin/tini", "--"]
CMD sh -l -c /swarm.sh
