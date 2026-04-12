ARG ALPINE_VERSION=3.23

FROM alpine:$ALPINE_VERSION AS builder
# Install development stuff
ARG MIRROR_ALPINE=""
ARG MIRROR_GITHUB=""
ARG MIRROR_QT=""
RUN if [ -n "$MIRROR_ALPINE" ]; then \
        sed -i "s|https://[^/]*/alpine/|$MIRROR_ALPINE/|g" /etc/apk/repositories; \
    fi && \
    apk --no-cache upgrade -a && \
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
        -DDESTINATION_DIR=/app \
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
        -DCMAKE_INSTALL_PREFIX=/app \
        -GNinja && \
    cmake --build /build/app && cmake --install /build/app && \
    ccache -s -vv && rm -rf /build

RUN strip /app/bin/AusweisApp && \
    find /app/lib/ -type f -not -name "*.so*" -delete && \
    find /app/ -type f -name "*.so*" -exec strip {} + && \
    find /app/ -type d -empty -delete

# Build clean file system
RUN mkdir /chroot && \
    cp -r --parents /etc/apk /chroot && \
    cp -r --parents /app/lib /chroot && \
    cp -r --parents /app/share /chroot && \
    cp --parents /app/plugins/tls/libqopensslbackend.so /chroot && \
    cp --parents /app/bin/AusweisApp /chroot && \
    install -d -m 777 /chroot/tmp && \
    apk --no-cache --root /chroot --initdb add \
        busybox-static alpine-baselayout-data libstdc++ pcsc-lite-libs eudev-libs && \
    mkdir -p /chroot/home/ausweisapp/.config && \
    mkdir -p /chroot/usr/local/bin && \
    mkdir -p /chroot/usr/local/share && \
    ln -s /app/bin/AusweisApp /chroot/usr/local/bin && \
    ln -s /app/share/* /chroot/usr/local/share && \
    chroot /chroot busybox.static adduser ausweisapp -D && \
    chroot /chroot busybox.static chown ausweisapp: /home/ausweisapp/.config && \
    chroot /chroot busybox.static rm -rf /etc/apk /lib/apk /var /bin

FROM scratch
COPY --from=builder /chroot/ /

USER ausweisapp
VOLUME ["/home/ausweisapp/.config"]
ENTRYPOINT ["/usr/local/bin/AusweisApp"]
EXPOSE 24727
CMD ["--address", "0.0.0.0", "--no-logfile"]
