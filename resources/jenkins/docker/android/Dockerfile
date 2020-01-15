FROM ubuntu:swarm
MAINTAINER Governikus KG <ausweisapp2@governikus.com>

ARG ANDROID_SDK_TOOLS=4333796
ARG ANDROID_NDK_VERSION=r18b
ENV NAME=Android LABELS="Android" PACKAGES_DIR=/home/governikus/packages

RUN apt-get update && \
    apt-get -y install g++ cmake make ccache ninja-build perl unzip gradle maven patch openjdk-8-jdk-headless && \
    ln -s /usr/lib/jvm/java-8-openjdk-amd64 /usr/lib/jvm/java-8-openjdk && \
    rm -rf /var/lib/apt/lists/*

ENV ANDROID_HOME /opt/android-sdk
RUN mkdir -p /tmp/dl/sdk && cd /tmp/dl/sdk && wget -O sdk.zip https://dl.google.com/android/repository/sdk-tools-linux-$ANDROID_SDK_TOOLS.zip && \
    unzip sdk.zip && mkdir -p $ANDROID_HOME && mv tools/ $ANDROID_HOME
RUN yes | $ANDROID_HOME/tools/bin/sdkmanager "build-tools;29.0.2" "platform-tools" "platforms;android-29"

ENV ANDROID_NDK_HOME /opt/android-ndk
ENV ANDROID_NDK=${ANDROID_NDK_HOME}

RUN mkdir -p /tmp/dl/ndk && \
    cd /tmp/dl/ndk && \
    wget -O ndk.zip https://dl.google.com/android/repository/android-ndk-${ANDROID_NDK_VERSION}-linux-x86_64.zip && \
    unzip ndk.zip && mv android-ndk-${ANDROID_NDK_VERSION} ${ANDROID_NDK} && \
    rm -rf /tmp/dl

USER governikus
RUN mkdir -p /home/governikus/.ccache && mkdir -p /home/governikus/workspace && mkdir -p /home/governikus/packages

ENTRYPOINT ["/usr/bin/tini", "--"]
CMD /swarm.sh
