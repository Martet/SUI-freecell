FROM ubuntu:20.04

# Install system packages
RUN DEBIAN_FRONTEND="noninteractive" apt-get update && apt-get -y install tzdata
RUN apt-get update \
    && apt-get install -y \
        openssh-server  \
        sudo \
        ssh \
    \
        python3-pip \
        python3-dev \
        python3-venv \
        libevent-dev \
    \
        vim \
    \
        gcc \
        g++ \
        gdb \
        clang \
        cmake \
        make \
        build-essential \
        autoconf \
        automake \
        valgrind \
        software-properties-common \
    \
       ninja-build \
       neovim \
       swig \
   \
       locales-all \
       dos2unix \
   \
       doxygen \
       fish \
        rsync \
        tar \
        tree \
        wget \
    && apt-get clean

WORKDIR /home/app

ENV SHELL /usr/bin/fish
CMD ["/bin/fish"]
#
## Setup root
#RUN echo 'root:root' | chpasswd
#
## Setup SSH: username=user:password=user
#RUN useradd -rm -d /home/user1 -s /bin/bash -g root -G sudo -u 1000 user1
#RUN echo 'user1:user1' | chpasswd
#RUN service ssh start
#
## USER user
#
## Port to expose and Entrypoint
#EXPOSE 22
#CMD ["/usr/sbin/sshd","-D"]