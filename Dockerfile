FROM puchnina/centos-devset:latest

MAINTAINER Puchnina Anastasia

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