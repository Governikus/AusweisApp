FROM alpine:swarm
MAINTAINER Governikus KG <ausweisapp2@governikus.com>

ENV NAME=Linux LABELS="Linux g++ clang++" PACKAGES_DIR=/home/governikus/packages

RUN apk --no-cache add cmake make g++ clang clang-analyzer ccache cloc cppcheck pkgconf ninja pcsc-lite-dev binutils-gold \
                       mesa-dev libx11-dev libxkbcommon-dev xcb-util-wm-dev xcb-util-image-dev xcb-util-keysyms-dev \
                       py2-pip && \
    ln -s /usr/libexec/c++-analyzer /usr/local/bin && ln -s /usr/libexec/ccc-analyzer /usr/local/bin && \
    pip install gcovr && apk del py2-pip

USER governikus
RUN mkdir -p /home/governikus/.ccache && mkdir -p /home/governikus/workspace && mkdir -p /home/governikus/packages
VOLUME /home/governikus/.ccache

ENTRYPOINT ["/sbin/tini", "--"]
CMD /swarm.sh
