ARG FROM=debian:buster
FROM ${FROM}

RUN \
  echo "debconf debconf/frontend select Noninteractive" | \
    debconf-set-selections

ARG DEBUG

RUN \
  quiet=$([ "${DEBUG}" = "yes" ] || echo "-qq") && \
  apt update ${quiet} && \
  apt install -y -V ${quiet} \
    autotools-dev \
    build-essential \
    debhelper \
    devscripts \
    gtk-doc-tools \
    intltool \
    libgoffice-0.10-dev \
    libgtk2.0-dev \
    libsoup2.4-dev \
    lsb-release \
    pkg-config && \
  apt clean && \
  rm -rf /var/lib/apt/lists/*
