FROM dev-docker.governikus.de/ausweisapp2/alpine:swarm

ENV NAME=Linux LABELS="Linux g++ clang++ clazy" PACKAGES_DIR=/home/governikus/packages

RUN apk --no-cache add g++ clang clang-analyzer ccache gcovr cloc clazy cppcheck pkgconf pcsc-lite-dev binutils-gold lld uncrustify dos2unix typos \
                       py3-codespell py3-setuptools check-jsonschema mesa-dev libx11-dev libxkbcommon-dev xcb-util-wm-dev xcb-util-image-dev xcb-util-keysyms-dev && \
    ln -s /usr/libexec/c++-analyzer /usr/local/bin && ln -s /usr/libexec/ccc-analyzer /usr/local/bin

USER governikus
RUN mkdir -p /home/governikus/.ccache && mkdir -p /home/governikus/workspace && mkdir -p /home/governikus/packages

ENTRYPOINT ["/sbin/tini", "--"]
CMD /swarm.sh
