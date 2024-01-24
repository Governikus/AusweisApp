FROM dev-docker.governikus.de/ausweisapp2/ubuntu:swarm
MAINTAINER Governikus KG <support@ausweisapp.de>

ENV NAME=Vanilla LABELS="Vanilla" PACKAGES_DIR=/home/governikus/packages

RUN apt-get update && \
    apt-get -y install cmake make g++ clazy clang clang-tidy ccache gcovr cloc pkg-config ninja-build binutils-gold lld \
                       valgrind tree libpcsclite-dev libhttp-parser-dev libssl-dev libudev-dev \
                       \
                       qml-module-qt-labs-platform qml-module-qtquick-controls2 qml-module-qtquick-controls2 qml-module-qttest \
                       qml-module-qtquick-layouts qml-module-qtqml-models2 \
                       qtdeclarative5-dev qtquickcontrols2-5-dev qttools5-dev libqt5svg5-dev \
                       libqt5websockets5-dev qtconnectivity5-dev \
                       \
                       libqt6opengl6-dev \
                       libqt6shadertools6-dev \
                       libqt6svg6-dev \
                       libqt6websockets6-dev \
                       qt6-base-dev \
                       qt6-base-private-dev \
                       qt6-connectivity-dev \
                       qt6-declarative-dev \
                       qt6-scxml-dev \
                       qt6-tools-dev \
                       qt6-tools-dev-tools \
                       qt6-l10n-tools \
                       qml6-module-qt-labs-platform \
                       qml6-module-qtqml \
                       qml6-module-qtqml-models \
                       qml6-module-qtqml-statemachine \
                       qml6-module-qtqml-workerscript \
                       qml6-module-qtquick-controls \
                       qml6-module-qtquick-layouts \
                       qml6-module-qtquick-templates \
                       qml6-module-qtquick-window \
                       && \
    rm -rf /var/lib/apt/lists/*

USER governikus
RUN mkdir -p /home/governikus/.ccache && mkdir -p /home/governikus/workspace && mkdir -p /home/governikus/packages

ENTRYPOINT ["/usr/bin/tini", "--"]
CMD /swarm.sh
