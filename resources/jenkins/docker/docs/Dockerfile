FROM alpine:swarm
MAINTAINER Governikus KG <ausweisapp2@governikus.com>

ENV NAME=Docs LABELS=Docs

RUN apk --no-cache add cmake make py3-sphinx py3-setuptools icu-libs poppler zziplib texlive-full && \
    pip3 install doc8 cloud_sptheme

USER governikus

ENTRYPOINT ["/sbin/tini", "--"]
CMD sh -l -c /swarm.sh
