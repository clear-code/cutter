# -*- rd -*-

= Install to Cygwin --- How to install Cutter to Cygwin

== Introduction

This document explains how to install Cutter to Cygwin.

We use Cygwin 1.5. We can install Cutter to Cygwin 1.7 but
we need to apply a patch to GLib.

== Install Cygwin

First, we download Cygwin 1.5 installer:

((<URL:http://cygwin.com/setup.exe>))

Next, we install the following packages with the downloaded
installer:

   * gcc
   * gdb
   * gettext-devel
   * intltool
   * libiconv
   * libtool
   * make
   * pkg-config
   * wget

We enter into Cygwin. We run Cygwin.

The latest GLib can't be built on Cygwin 1.5. We use old
GLib that is supported by Cutter.

We install GLib into /usr/local/.

  % wget http://ftp.gnome.org/pub/gnome/sources/glib/2.16/glib-2.16.6.tar.gz
  % tar xvzf glib-2.16.6.tar.gz
  % cd glib-2.16.6
  % ./configure
  % make
  % make install

== Install Cutter

We install Cutter.

  % cd
  % wget http://downloads.sourceforge.net/cutter/cutter-1.0.7.tar.gz
  % tar xvzf cutter-1.0.7.tar.gz
  % cd cutter-1.0.7
  % ./configure PKG_CONFIG_PATH=/usr/local/lib/pkgconfig
  % make
  % make install

== The next step

Installation is completed. We should try
((<tutorial|TUTORIAL>)) with the installed Cutter.
