FROM dev-docker.governikus.de/ausweisapp2/alpine:swarm

ENV NAME=Sync LABELS=Sync

RUN apk --no-cache add openssh-client rsync py3-pillow py3-curl py3-progressbar2 py3-jsondiff

USER governikus
RUN echo -e "[extensions]\nlargefiles=" > ~/.hgrc

ENTRYPOINT ["/sbin/tini", "--"]
CMD /swarm.sh
