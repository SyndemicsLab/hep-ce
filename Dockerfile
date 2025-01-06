# GCC support can be specified at major, minor, or micro version
# (e.g. 8, 8.2 or 8.2.0).
# See https://hub.docker.com/r/library/gcc/ for all supported GCC
# tags from Docker Hub.
# See https://docs.docker.com/samples/library/gcc/ for more on how to use this image
FROM ubuntu:22.04 AS builder

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive \
    apt-get install --assume-yes --no-install-recommends \
    cmake \
    libboost-all-dev \
    sqlite3 

COPY src .
COPY scripts .
RUN mkdir build

RUN ./scripts/build.sh -t Release

# This command runs your application, comment out this line to compile only
ENTRYPOINT [ "bin/hepce" ]

LABEL Name=hepcesimulationv2 Version=0.0.1
