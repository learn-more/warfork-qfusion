# Run the following command and a build archive will be produced in the build/ directory:
# docker build --output build .
FROM ubuntu:20.04 AS build
ARG DEBIAN_FRONTEND=noninteractive
ARG DEBCONF_NONINTERACTIVE_SEEN=true
RUN apt update
RUN apt install -y curl clang cmake build-essential \
    libsdl2-dev libopenal-dev libvorbis-dev libtheora-dev \
    libfreetype6-dev libcurl4-gnutls-dev git zip unzip \
    && rm -rf /var/lib/apt/lists/*
RUN mkdir -p /root/warfork
WORKDIR /root/warfork
COPY .clang-format .
COPY icons icons
COPY libsrcs libsrcs
COPY third-party third-party
COPY .git .
COPY source source
RUN curl https://warfork.com/downloads/sdk/ --output third-party/steamworks/sdk.zip
RUN unzip third-party/steamworks/sdk.zip -d third-party/steamworks
RUN export CC=clang CXX=clang++
RUN cmake -DCMAKE_BUILD_TYPE=DEBUG -DBUILD_STEAMLIB=1 ./source
RUN make -j8
RUN tar -czvf ./source/Linux-x86_64-Debug.tar.gz ./source/build/*
FROM scratch AS export
COPY --from=build /root/warfork/source/Linux-x86_64-Debug.tar.gz .