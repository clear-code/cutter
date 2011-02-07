# -*- rd -*-

= Install to Cygwin --- How to install Cutter to Cygwin

== Introduction

This document explains how to install Cutter to Cygwin.

We can install Cutter to Cygwin 1.7 (should be confirmed:
but we need to apply a patch to GLib).

== Install Cygwin

First, we download Cygwin 1.7 installer:

((<URL:http://cygwin.com/setup.exe>))

Next, we install the following packages with the downloaded
installer:

   * gcc4
   * gdb
   * libglib2.0-devel
   * gettext-devel
   * intltool
   * libiconv
   * libtool
   * make
   * pkg-config
   * wget

We enter into Cygwin. We run Cygwin.

== Install Cutter

We install Cutter.

  % cd
  % wget http://downloads.sourceforge.net/cutter/cutter-1.1.6.tar.gz
  % tar xvzf cutter-1.1.6.tar.gz
  % cd cutter-1.1.6
  % ./configure
  % make
  % make install

== The next step

Installation is completed. We should try
((<tutorial|TUTORIAL>)) with the installed Cutter.
