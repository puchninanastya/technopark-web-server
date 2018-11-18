FROM centos:7

MAINTAINER Puchnina Anastasia

# Setting up environment
RUN yum install -y sudo
RUN sudo yum install -y centos-release-scl
RUN sudo yum install -y devtoolset-6
RUN yum install -y wget
RUN sudo yum group install -y "Development Tools"

# Installing CMake
ENV CMAKE_VERSION 3.8
ENV CMAKE_VERSION_FULL 3.8.2
RUN wget https://cmake.org/files/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION_FULL}.tar.gz
RUN tar -zxvf cmake-${CMAKE_VERSION_FULL}.tar.gz
RUN cd cmake-${CMAKE_VERSION_FULL} && \
    sudo ./bootstrap --prefix=/usr/local && \
    sudo make && \
    sudo make install

# Setting up project folder
RUN mkdir -p /usr/src/monzza
WORKDIR /usr/src/monzza
COPY . .

# Compiling monzza web server
RUN cmake -DCMAKE_BUILD_TYPE=Release CMakeLists.txt
RUN make
RUN mkdir -p logs

EXPOSE 80

CMD sudo ./monzza