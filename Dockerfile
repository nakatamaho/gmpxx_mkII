# * How to build
#  docker build -f Dockerfile --build-arg SSH_KEY="$(cat ~/.ssh/id_ed25519)" -t gmpxx_mkii .
# * How to run
#  sudo sh -c 'echo -1 >/proc/sys/kernel/perf_event_paranoid' # to run perf
#  or
#  sudo sysctl -w kernel.perf_event_paranoid=-1 # to run perf
#  docker run --privileged -it gmpxx_mkii /bin/bash
#
FROM ubuntu:22.04

LABEL maintainer="maho.nakata@gmail.com"

RUN apt -y update
RUN apt -y upgrade
RUN apt install -y sudo
RUN apt install -y tzdata
# set your timezone
ENV TZ Asia/Tokyo
RUN echo "${TZ}" > /etc/timezone \
  && rm /etc/localtime \
  && ln -s /usr/share/zoneinfo/Asia/Tokyo /etc/localtime \
  && dpkg-reconfigure -f noninteractive tzdata

RUN apt install -y build-essential python3 gcc-12 g++-12 gfortran-12
RUN apt install -y autotools-dev automake libtool libtool-bin gnuplot
RUN apt install -y gdb valgrind linux-tools-`uname -r` google-perftools linux-headers-`uname -r`
RUN apt install -y git wget ccache time parallel bc
RUN apt install -y pkg-config clangd clang-format unifdef octave
RUN apt install -y ng-common ng-cjk emacs-nox
RUN update-alternatives --install /usr/bin/python python /usr/bin/python3 1

ARG DOCKER_UID=1000
ARG DOCKER_USER=docker
ARG DOCKER_PASSWORD=docker
RUN useradd -u $DOCKER_UID -m $DOCKER_USER --shell /bin/bash && echo "$DOCKER_USER:$DOCKER_PASSWORD" | chpasswd && echo "$DOCKER_USER ALL=(ALL) ALL" >> /etc/sudoers
ARG WORK=/home/$DOCKER_USER

USER docker
USER ${DOCKER_USER}

# Authorize SSH Host
RUN mkdir -p /home/$DOCKER_USER/.ssh && \
    chmod 0700 /home/$DOCKER_USER/.ssh && \
    ssh-keyscan github.com > /home/$DOCKER_USER/.ssh/known_hosts
# Add the keys and set permissions
ARG SSH_KEY
RUN echo "$SSH_KEY" > /home/$DOCKER_USER/.ssh/id_ed25519
RUN chmod 600 /home/$DOCKER_USER/.ssh/id_ed25519

RUN cd ${WORK} && echo "cd /home/$DOCKER_USER" >> .bashrc
RUN cd ${WORK} && echo 'eval "$(ssh-agent -s)"' >> .bashrc
RUN cd ${WORK} && echo 'ssh-add ~/.ssh/id_ed25519' >> .bashrc

# setup github and clone
#git setting
ARG GIT_EMAIL="maho.nakata@gmail.com"
ARG GIT_NAME="NAKATA Maho"
RUN echo "\n\
[user]\n\
        email = ${GIT_EMAIL}\n\
        name = ${GIT_NAME}\n\
" > /home/$DOCKER_USER/.gitconfig
SHELL ["/bin/bash", "-c"]

RUN cd ${WORK} && git clone https://github.com/brendangregg/FlameGraph.git
RUN cd ${WORK} && git clone https://github.com/nakatamaho/gmpxx_mkII.git
RUN cd ${WORK}/gmpxx_mkII && git remote set-url origin git@github.com:nakatamaho/gmpxx_mkII.git
RUN cd ${WORK}/gmpxx_mkII/setup && bash -x setup_gmp.sh
#RUN cd ${WORK}/gmpxx_mkII && make
