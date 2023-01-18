FROM alpine:swarm
MAINTAINER Governikus KG <support@ausweisapp.de>

ENV NAME=Docs LABELS=Docs

RUN apk --no-cache add py3-setuptools icu poppler zziplib enscript ghostscript texlive-full py3-sphinx py3-sphinx_rtd_theme && \
    pip3 install doc8

USER governikus

ENTRYPOINT ["/sbin/tini", "--"]
CMD sh -l -c /swarm.sh
