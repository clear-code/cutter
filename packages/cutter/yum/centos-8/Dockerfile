FROM centos:8

ARG DEBUG

RUN \
  quiet=$([ "${DEBUG}" = "yes" ] || echo "--quiet") && \
  dnf update -y ${quiet} && \
  dnf install -y ${quiet} \
    epel-release \
    'dnf-command(config-manager)' && \
  dnf config-manager --set-enabled PowerTools && \
  dnf groupinstall -y ${quiet} "Development Tools" && \
  dnf install -y ${quiet} \
    gcc \
    gcc-c++ \
    gettext \
    glib2-devel \
    gtk-doc \
    gtk2-devel \
    intltool \
    libsoup-devel \
    make && \
  dnf clean ${quiet} all
