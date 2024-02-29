FROM dev-docker.governikus.de/ausweisapp2/ubuntu:swarm
MAINTAINER Governikus KG <support@ausweisapp.de>

ARG ANDROID_CMDLINE_TOOLS=11076708
ARG ANDROID_NDK_VERSION=26.2.11394342
ARG CMAKE=3.28.3
ENV NAME=Android LABELS="Android" PACKAGES_DIR=/home/governikus/packages

ENV ANDROID_SDK_ROOT /opt/android-sdk
ENV ANDROID_NDK_ROOT $ANDROID_SDK_ROOT/ndk/$ANDROID_NDK_VERSION

RUN apt-get update && \
    apt-get -y install g++ make ccache ninja-build perl unzip gradle maven patch openjdk-17-jdk-headless && \
    rm -rf /var/lib/apt/lists/*

RUN wget https://github.com/Kitware/CMake/releases/download/v$CMAKE/cmake-$CMAKE-Linux-x86_64.sh -O /tmp/cmake.sh && \
    sh /tmp/cmake.sh --prefix=/usr --skip-license --exclude-subdir && rm /tmp/cmake.sh

RUN mkdir -p /tmp/dl && cd /tmp/dl && wget -O sdk.zip https://dl.google.com/android/repository/commandlinetools-linux-${ANDROID_CMDLINE_TOOLS}_latest.zip && \
    unzip sdk.zip && \
    yes | /tmp/dl/cmdline-tools/bin/sdkmanager --sdk_root=$ANDROID_SDK_ROOT "cmdline-tools;12.0" "build-tools;34.0.0" "platforms;android-34" "ndk;${ANDROID_NDK_VERSION}" && \
    rm -rf /tmp/dl

USER governikus
RUN mkdir -p /home/governikus/.ccache && mkdir -p /home/governikus/workspace && mkdir -p /home/governikus/packages && mkdir -p /home/governikus/.android

ENTRYPOINT ["/usr/bin/tini", "--"]
CMD /swarm.sh
