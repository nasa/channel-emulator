FROM ubuntu:14.04.4

RUN apt-get update -y && apt-get install wget  \
                                 libcurl4-openssl-dev \
                                 libwww-perl \
                                 sudo \
                                 libconfig++9 \
                                 libnl-route-3-200 \
                                 libboost-regex-dev \
                                 libnl-3-dev libnl-route-3-dev libnl-nf-3-dev libnl-genl-3-dev \
                                 libmagic-dev \
                                 cmake \
                                 g++ \
                                 libconfig-dev \
                                 libxmlrpc-c++8 \
                                 git \
                                 -y

RUN wget --no-check-certificate https://sourceforge.net/projects/xmlrpc-c/files/Xmlrpc-c%20Super%20Stable/1.54.06/xmlrpc-c-1.54.06.tgz && \
    tar -xzf xmlrpc-c-1.54.06.tgz && \
    rm xmlrpc-c-1.54.06.tgz && \
    cd xmlrpc-c-1.54.06 && \
    ./configure && \
    make -j $(nproc) && \
    make install

RUN wget https://github.com/nasa/channel-emulator/releases/download/v1.8.6.5/libace.tmb-6.0.1_6.0.1-3_amd64.deb&& \
    sudo dpkg -i libace.tmb-6.0.1_6.0.1-3_amd64.deb && \
    rm libace.tmb-6.0.1_6.0.1-3_amd64.deb && \
    sudo apt-get install -f

RUN wget https://github.com/nasa/channel-emulator/releases/download/v1.8.6.5/libace.tmb-dev_6.0.1-3_amd64.deb && \
    sudo dpkg -i libace.tmb-dev_6.0.1-3_amd64.deb && \
    rm libace.tmb-dev_6.0.1-3_amd64.deb && \
    sudo apt-get install -f

RUN wget https://github.com/nasa/channel-emulator/releases/download/v1.8.6.5/ctsce_1.8.6.5_amd64.deb && \
    sudo dpkg -i ctsce_1.8.6.5_amd64.deb && \
    rm ctsce_1.8.6.5_amd64.deb && \
    sudo apt-get install -f

RUN git clone -b channel-emulator-fixes https://github.com/bilaljo/channel-emulator.git

WORKDIR /channel-emulator

RUN cmake .
