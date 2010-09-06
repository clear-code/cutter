# -*- rd -*-

= Install to Solaris --- How to install Cutter to Solaris

== Introduction

This document explains how to install Cutter to Solaris.

== Install GLib

Cutter requires GLib 2.16 or later. There is no package for
GLib 2.16 or later for Solaris 10. We need to build and
install by ourself.

GLib requires GNU gettext and GNU libiconv. We can install
them as packages at
((<Sunfreeware|URL:http://www.sunfreeware.com/>)).

For example, we can install GNU gettext with the following
steps:

  % mkdir -p ~/packages
  % cd ~/packages
  % wget ftp://ftp.sunfreeware.com/pub/freeware/sparc/10/gettext-0.17-sol10-sparc-local.gz
  % gzip -d gettext-0.17-sol10-sparc-local.gz
  % pkgadd -d ./gettext-0.17-sol10-sparc-local

We can install GNU libiconv with the same steps.

We also need GNU build system, GNU Autoconf, GNU Automake
and GNU Libtool, on development with Cutter. We need to
install them. It's a good idea that we also install
intltool. It's not required but it's needed for
internationalized software development.

We can build GLib after we install the above packages. Here
are steps to build and install GLib. Note that we need to
use GNU make:

  % mkdir -p ~/src
  % cd ~/src
  % wget http://ftp.gnome.org/pub/GNOME/sources/glib/2.22/glib-2.22.4.tar.gz
  % tar xvzf glib-2.22.4.tar.gz
  % cd glib-2.22.4
  % ./configure --with-libiconv=gnu
  % gmake
  % sudo gmake install

== Install Cutter

Cutter can be installed like other free software because
Cutter uses GNU build tools. Note that we need to use GNU
make:

  % mkdir -p ~/src
  % cd ~/src
  % wget http://downloads.sourceforge.net/cutter/cutter-1.1.5.tar.gz
  % tar xvzf cutter-1.1.5.tar.gz
  % cd cutter-1.1.5
  % ./configure
  % gmake
  % sudo gmake install

== The next step

Installation is completed. We should try
((<tutorial|TUTORIAL>)) with the installed Cutter.
