# Pull base image
FROM ubuntu:latest

# Install packages
RUN \
  apt-get update && \
  apt-get -y upgrade && \
  apt-get install -y git cmake gcc g++ \
                     freeglut3-dev libfreetype6-dev libxrandr-dev \
                     libxinerama-dev libxcursor-dev
