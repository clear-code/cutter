FROM debian:buster

RUN \
  echo "debconf debconf/frontend select Noninteractive" | \
    debconf-set-selections

ARG DEBUG

RUN \
  quiet=$([ "${DEBUG}" = "yes" ] || echo "-qq") && \
  apt update ${quiet} && \
  apt install -y -V ${quiet} \
    debhelper \
    devscripts \
    gnupg && \
  apt clean && \
  rm -rf /var/lib/apt/lists/*
