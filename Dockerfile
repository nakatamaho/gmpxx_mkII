# Prerequisites:
#   - SSH key for GitHub access: ~/.ssh/id_ed25519
#
# Build:
#   docker build -f Dockerfile --build-arg GIT_COMMIT_TRIGGER=$(date +%s) --build-arg SSH_KEY="$(cat ~/.ssh/id_ed25519)" -t gmpxx_mkii .
#
# Run:
#   docker run -it --rm gmpxx_mkii
#
# Development:
#   docker run -it --rm -v $(pwd):/workspace gmpxx_mkii
#
# Performance profiling with perf:
#   sudo sysctl -w kernel.perf_event_paranoid=-1
#   docker run -it --rm --privileged gmpxx_mkii
#   # Inside container: perf record ./your_benchmark && perf report

FROM ubuntu:24.04

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

RUN apt install -y build-essential python3 gcc g++ gfortran
RUN apt install -y autotools-dev automake libtool libtool-bin gnuplot cmake
RUN apt install -y gdb valgrind linux-tools-`uname -r` google-perftools linux-headers-`uname -r`
RUN apt install -y git wget ccache time parallel bc
RUN apt install -y pkg-config clangd clang-format unifdef octave
RUN apt install -y ng-common ng-cjk emacs-nox
RUN apt install -y python3-matplotlib
RUN update-alternatives --install /usr/bin/python python /usr/bin/python3 1

#clang-format-19
RUN apt update && apt install -y wget gnupg lsb-release
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | gpg --dearmor -o /usr/share/keyrings/llvm-archive-keyring.gpg
RUN echo "deb [signed-by=/usr/share/keyrings/llvm-archive-keyring.gpg] http://apt.llvm.org/jammy/ llvm-toolchain-jammy-19 main" \
    > /etc/apt/sources.list.d/llvm.list
RUN apt update && apt install -y clang-format-19

# Create user (use random password for security)
ARG DOCKER_UID=1001
ARG DOCKER_USER=docker
RUN useradd -u $DOCKER_UID -m $DOCKER_USER --shell /bin/bash \
    && echo "$DOCKER_USER ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

USER $DOCKER_USER
WORKDIR /home/$DOCKER_USER

# Setup SSH for GitHub (supports both BuildKit secrets and legacy ARG)
RUN mkdir -p ~/.ssh && chmod 700 ~/.ssh \
    && ssh-keyscan github.com > ~/.ssh/known_hosts

# Method 1: BuildKit secrets (recommended)
RUN --mount=type=secret,id=ssh_key,target=/tmp/ssh_key \
    if [ -f /tmp/ssh_key ]; then \
        cp /tmp/ssh_key ~/.ssh/id_ed25519 && chmod 600 ~/.ssh/id_ed25519; \
    fi

# Method 2: Legacy ARG (fallback - less secure)
ARG SSH_KEY=""
RUN if [ -n "$SSH_KEY" ]; then \
        echo "$SSH_KEY" > ~/.ssh/id_ed25519 && chmod 600 ~/.ssh/id_ed25519; \
    fi

# Setup git
ARG GIT_EMAIL="maho.nakata@gmail.com"
ARG GIT_NAME="NAKATA Maho"
RUN git config --global user.email "$GIT_EMAIL" \
    && git config --global user.name "$GIT_NAME"

# Setup SSH agent in bashrc
RUN echo 'eval "$(ssh-agent -s)" && ssh-add ~/.ssh/id_ed25519 2>/dev/null' >> ~/.bashrc

ARG GIT_COMMIT_TRIGGER=unspecified

RUN git clone https://github.com/brendangregg/FlameGraph.git
RUN git clone https://github.com/nakatamaho/gmpxx_mkII.git
RUN cd gmpxx_mkII && git remote set-url origin git@github.com:nakatamaho/gmpxx_mkII.git
RUN cd gmpxx_mkII/setup && bash -x setup_gmp.sh
RUN cd gmpxx_mkII && make
