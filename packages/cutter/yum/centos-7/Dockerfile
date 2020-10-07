FROM centos:7

ARG DEBUG

RUN \
  quiet=$([ "${DEBUG}" = "yes" ] || echo "--quiet") && \
  yum update -y ${quiet} && \
  yum install -y ${quiet} epel-release && \
  yum groupinstall -y ${quiet} "Development Tools" && \
  yum install -y ${quiet} \
    gcc \
    gcc-c++ \
    gettext \
    glib2-devel \
    gtk-doc \
    gtk2-devel \
    intltool \
    libsoup-devel \
    make && \
  yum clean ${quiet} all
