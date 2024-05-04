# ARG TARGET=Debug

FROM ubuntu:24.04

RUN apt-get update
RUN apt-get install -y gcc
RUN apt-get install -y cmake
RUN apt-get install -y python3
RUN apt-get install -y pip
RUN pip install conan --break-system-packages

COPY . /source

RUN cd /source && mkdir -p .conan && cd .conan && conan profile detect && conan install .. -s build_type=Debug -s compiler.libcxx=libstdc++11 --build missing
RUN cd /source && cmake . -DCMAKE_TOOLCHAIN_FILE=build/Debug/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
RUN cd /source && make

# RUN cd /source && chmod +x path-finding
# RUN cd /source && ./path-finding

# RUN cd /source && chmod +x path-finding-test
# RUN cd /source && ./path-finding-test