FROM dev-docker.governikus.de/ausweisapp2/alpine:swarm
MAINTAINER Governikus KG <support@ausweisapp.de>

ENV NAME=Docs LABELS=Docs

RUN apk --no-cache add py3-setuptools icu poppler zziplib enscript ghostscript texlive-full py3-sphinx py3-sphinx_rtd_theme py3-sphinx-copybutton && \
    pip3 install --break-system-packages doc8

USER governikus

ENTRYPOINT ["/sbin/tini", "--"]
CMD sh -l -c /swarm.sh
