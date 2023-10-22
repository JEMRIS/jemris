FROM ubuntu:22.04

RUN apt update && \
    apt install -y libxerces-c-dev build-essential\
    libsundials-dev libginac-dev ginac-tools \
    libginac-dev libginac11 libhdf5-dev libismrmrd-dev mpi-default-bin libboost-all-dev
RUN mkdir -p /workspace && chmod u+rwx /workspace

WORKDIR /workspace

COPY . /workspace/

RUN cmake -B ./build -DCMAKE_BUILD_TYPE=Release -DSKIP_CONDA=1
RUN cd ./build && make -j4 && make install

RUN useradd -rm -d /home/jemris -s /bin/bash -g root -u 1001 jemris 
USER jemris

RUN mkdir /home/jemris/sim && chmod u+rwx /home/jemris/sim
VOLUME /home/jemris/sim
WORKDIR /home/jemris/sim

ENTRYPOINT [ "jemris" ]