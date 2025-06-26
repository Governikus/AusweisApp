FROM dev-docker.governikus.de/ausweisapp2/alpine:swarm

ENV NAME=Common LABELS=Common

USER governikus

ENTRYPOINT ["/sbin/tini", "--"]
CMD /swarm.sh
