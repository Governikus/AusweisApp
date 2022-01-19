FROM alpine:swarm
MAINTAINER Governikus KG <support@ausweisapp.de>

ENV NAME=Trigger LABELS="Trigger Seeder"

USER governikus

ENTRYPOINT ["/sbin/tini", "--"]
CMD /swarm.sh
