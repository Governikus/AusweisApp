FROM alpine:swarm
MAINTAINER Governikus KG <ausweisapp2@governikus.com>

ENV NAME=Docs LABELS=Docs

# Clean up if texlive package is fixed in Alpine
RUN sed -i -e 's/v3.7/edge/' /etc/apk/repositories && \
    echo '@testing http://dl-cdn.alpinelinux.org/alpine/edge/testing' >> /etc/apk/repositories && \
    apk --no-cache add cmake make py2-sphinx py2-setuptools py2-pip py2-hglib icu-libs poppler zziplib texlive-full@testing && \
    wget http://ftp.math.utah.edu/pub/tex/historic/systems/texlive/2016/texlive-20160523b-texmf.tar.xz && \
    cmake -E tar xf texlive-20160523b-texmf.tar.xz && \
    cp -r /texlive-20160523-texmf/texmf-dist /usr/share && \
    rm -rf texlive* && \
    apk --no-cache fix texlive && \
    ln -s /usr/bin/mktexlsr /usr/bin/mktexlsr.pl && \
    pip install doc8 cloud_sptheme

USER governikus

ENTRYPOINT ["/sbin/tini", "--"]
CMD sh -l -c /swarm.sh
