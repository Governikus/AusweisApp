ARG ALPINE_VERSION=3.17

FROM alpine:$ALPINE_VERSION AS builder
# Install development stuff
RUN apk --no-cache upgrade -a && \
    apk --no-cache add patch cmake ccache make ninja g++ pkgconf pcsc-lite-dev binutils-gold eudev-libs \
        http-parser-dev openssl-dev \
        qt6-qtbase-dev qt6-qtsvg-dev qt6-qtwebsockets-dev qt6-qttools-dev qt6-qtdeclarative-dev qt6-qtscxml-dev qt6-qtconnectivity-dev qt6-qtimageformats-dev qt6-qtimageformats

# Use optional remote ccache
# redis://YOUR_SERVER:6379
ARG CCACHE_REMOTE_STORAGE=""
ENV CCACHE_REMOTE_STORAGE=$CCACHE_REMOTE_STORAGE CCACHE_REMOTE_ONLY=true CCACHE_RESHARE=true CCACHE_DIR=/build/ccache XDG_RUNTIME_DIR=/root

# Build AusweisApp
COPY README.rst /src/ausweisapp/
COPY LICENSE.txt/ /src/ausweisapp/
COPY LICENSE.officially.txt/ /src/ausweisapp/
COPY docs/ /src/ausweisapp/docs/
COPY CMakeLists.txt /src/ausweisapp/
COPY cmake/ /src/ausweisapp/cmake/
COPY resources/ /src/ausweisapp/resources/
COPY src/ /src/ausweisapp/src/
COPY test/ /src/ausweisapp/test/

RUN cmake /src/ausweisapp -B /build/app \
        -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
        -DBUILD_SHARED_LIBS=ON \
        -GNinja && \
    cmake --build /build/app && \
    ctest --test-dir /build/app --output-on-failure -j `nproc` && \
    cmake --install /build/app && \
    ccache -s -vv && rm -rf /build

RUN find /usr/local/ -type d -empty -delete && \
    find /usr/local/lib/ -type f -not -name "*.so*" -delete && \
    find /usr/local/lib/ -type f -name "*.so*" -exec strip {} + && \
    strip /usr/local/bin/AusweisApp



FROM alpine:$ALPINE_VERSION
ENV XDG_RUNTIME_DIR=/home/ausweisapp QT_QPA_PLATFORM=vnc

COPY --from=builder /usr/local/lib /usr/local/lib
COPY --from=builder /usr/local/share /usr/local/share
COPY --from=builder /usr/local/bin/AusweisApp /usr/local/bin/AusweisApp

RUN apk --no-cache upgrade -a && \
    apk --no-cache add tini pcsc-lite-libs eudev-libs doas ttf-freefont \
        http-parser qt6-qtbase qt6-qtsvg qt6-qtwebsockets qt6-qtdeclarative qt6-qtscxml qt6-qtconnectivity qt6-qtimageformats && \
    echo 'permit nopass :wheel' > /etc/doas.d/wheel.conf && \
    adduser ausweisapp -G wheel -s /bin/sh -D && \
    chmod 0700 /home/ausweisapp && mkdir -p /home/ausweisapp/.config && chown ausweisapp: /home/ausweisapp/.config

USER ausweisapp
VOLUME ["/home/ausweisapp/.config"]
ENTRYPOINT ["/sbin/tini", "--"]
EXPOSE 5900/tcp 24727/tcp 24727/udp
CMD ["AusweisApp", "--address", "0.0.0.0", "--no-logfile"]
