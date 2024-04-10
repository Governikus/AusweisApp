ARG ALPINE_VERSION=3.19

FROM alpine:$ALPINE_VERSION as builder
# Install development stuff
RUN apk --no-cache upgrade -a && \
    apk --no-cache add patch cmake ccache make ninja g++ pkgconf pcsc-lite-dev binutils-gold eudev-libs perl python3 linux-headers

# Use optional remote ccache
# redis://YOUR_SERVER:6379
ARG CCACHE_REMOTE_STORAGE=""
ENV CCACHE_REMOTE_STORAGE=$CCACHE_REMOTE_STORAGE CCACHE_REMOTE_ONLY=true CCACHE_RESHARE=true CCACHE_DIR=/build/ccache

# Build Libraries
COPY cmake/Helper.cmake cmake/DVCS.cmake cmake/Messages.cmake libs /src/libs/

RUN cmake /src/libs/ -B /build/libs \
        -DCONTAINER_SDK=ON \
        -DCMAKE_BUILD_TYPE=Release \
        -DDESTINATION_DIR=/usr/local \
        -DCMAKE_CXX_COMPILER_LAUNCHER=ccache && \
    cmake --build /build/libs && \
    ccache -s -vv && rm -rf /build

# Build AusweisApp
COPY docs/ /src/ausweisapp/docs/
COPY CMakeLists.txt /src/ausweisapp/
COPY cmake/ /src/ausweisapp/cmake/
COPY resources/ /src/ausweisapp/resources/
COPY src/ /src/ausweisapp/src/

RUN cmake /src/ausweisapp -B /build/app \
        -DCONTAINER_SDK=ON \
        -DCMAKE_BUILD_TYPE=MinSizeRel \
        -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
        -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=ON \
        -GNinja && \
    cmake --build /build/app && cmake --install /build/app && \
    ccache -s -vv && rm -rf /build

RUN find /usr/local/ -type d -empty -delete && \
    find /usr/local/lib/ -type f -not -name "*.so*" -delete && \
    find /usr/local/lib/ -type f -name "*.so*" -exec strip {} + && \
    strip /usr/local/bin/AusweisApp



FROM alpine:$ALPINE_VERSION
# Copy to image
COPY --from=builder /usr/local/plugins /usr/local/plugins
COPY --from=builder /usr/local/lib /usr/local/lib
COPY --from=builder /usr/local/share /usr/local/share
COPY --from=builder /usr/local/bin/AusweisApp /usr/local/bin/AusweisApp

RUN apk --no-cache upgrade -a && \
    apk --no-cache add tini pcsc-lite pcsc-lite-libs ccid pcsc-cyberjack acsccid eudev-libs doas && \
    echo 'permit nopass :wheel' > /etc/doas.d/wheel.conf && \
    adduser ausweisapp -G wheel -s /bin/sh -D && \
    mkdir -p /home/ausweisapp/.config && chown ausweisapp: /home/ausweisapp/.config

USER ausweisapp
VOLUME ["/home/ausweisapp/.config"]
ENTRYPOINT ["/sbin/tini", "--"]
EXPOSE 24727
CMD ["AusweisApp", "--address", "0.0.0.0"]
