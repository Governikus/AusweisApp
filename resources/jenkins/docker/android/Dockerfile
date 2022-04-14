FROM ubuntu:swarm
MAINTAINER Governikus KG <support@ausweisapp.de>

ARG ANDROID_CMDLINE_TOOLS=8092744
ARG ANDROID_NDK_VERSION=21.4.7075529
ARG CMAKE=3.22.2
ENV NAME=Android LABELS="Android" PACKAGES_DIR=/home/governikus/packages

ENV ANDROID_SDK_ROOT /opt/android-sdk
ENV ANDROID_NDK_ROOT $ANDROID_SDK_ROOT/ndk/$ANDROID_NDK_VERSION

# Remove this later: https://github.com/openssl/openssl/pull/11206
ENV ANDROID_NDK $ANDROID_NDK_ROOT

RUN apt-get update && \
    apt-get -y install g++ make ccache ninja-build perl unzip gradle maven patch openjdk-11-jdk-headless && \
    rm -rf /var/lib/apt/lists/*

# CMake 3.16 is required for NDK r19+
RUN wget https://github.com/Kitware/CMake/releases/download/v$CMAKE/cmake-$CMAKE-Linux-x86_64.sh -O /tmp/cmake.sh && \
    sh /tmp/cmake.sh --prefix=/usr --skip-license --exclude-subdir && rm /tmp/cmake.sh

RUN mkdir -p /tmp/dl && cd /tmp/dl && wget -O sdk.zip https://dl.google.com/android/repository/commandlinetools-linux-${ANDROID_CMDLINE_TOOLS}_latest.zip && \
    unzip sdk.zip && \
    yes | /tmp/dl/cmdline-tools/bin/sdkmanager --sdk_root=$ANDROID_SDK_ROOT "cmdline-tools;6.0" "build-tools;30.0.3" "platforms;android-30" "platforms;android-31" "ndk;${ANDROID_NDK_VERSION}" && \
    rm -rf /tmp/dl

USER governikus
RUN mkdir -p /home/governikus/.ccache && mkdir -p /home/governikus/workspace && mkdir -p /home/governikus/packages && mkdir -p /home/governikus/.android

ENTRYPOINT ["/usr/bin/tini", "--"]
CMD /swarm.sh
