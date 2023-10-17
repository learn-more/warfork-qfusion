# Run the following command and a build archive will be produced in the build/ directory:
# docker build --output build .
FROM ubuntu:20.04 AS build
RUN export DEBIAN_FRONTEND=noninteractive
RUN export DEBCONF_NONINTERACTIVE_SEEN=true
RUN touch /tmp/nyc.txt
RUN echo "tzdata tzdata/Areas select America" > /tmp/nyc.txt
RUN echo "tzdata tzdata/Zones/America select New_York" >> /tmp/nyc.txt
RUN debconf-set-selections /tmp/nyc.txt
RUN ln -fs /usr/share/zoneinfo/America/New_York /etc/localtime
RUN apt-get update
RUN apt-get install -y --no-install-recommends tzdata
RUN apt-get install -y openssh-client git
RUN apt-get install -y curl clang cmake build-essential \
    libsdl2-dev libopenal-dev libvorbis-dev libtheora-dev \
    libfreetype6-dev libcurl4-gnutls-dev git zip unzip \
    && rm -rf /var/lib/apt/lists/*
RUN mkdir -p /root/warfork
WORKDIR /root/warfork
COPY . .
RUN curl https://warfork.com/downloads/sdk/ --output third-party/steamworks/sdk.zip
RUN unzip third-party/steamworks/sdk.zip -d third-party/steamworks
RUN export CC=clang CXX=clang++
RUN cmake -DCMAKE_BUILD_TYPE=DEBUG -DBUILD_STEAMLIB=1 ./source
RUN make -j8
RUN echo "${CIRCLE_SHA1}" > ./source/build/commit.txt
RUN echo "671610" > ./source/build/steam_appid.txt
RUN tar -czvf ./source/Linux-x86_64-Debug.tar.gz ./source/build/*
FROM scratch AS export
COPY --from=build /root/warfork/source/Linux-x86_64-Debug.tar.gz .