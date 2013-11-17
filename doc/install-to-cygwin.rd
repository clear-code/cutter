# -*- rd -*-

= Install to Cygwin --- How to install Cutter to Cygwin

== Introduction

This document explains how to install Cutter to Cygwin.

We can install Cutter to Cygwin 1.7.

== Install Cygwin

First, we download Cygwin 1.7 installer:

  * ((<Installer for 32-bit environment|URL:http://cygwin.com/setup-x86.exe>))
  * ((<Installer for 64-bit environment|URL:http://cygwin.com/setup-x86_64.exe>))

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
  % wget http://downloads.sourceforge.net/cutter/cutter-1.2.2.tar.gz
  % tar xvzf cutter-1.2.2.tar.gz
  % cd cutter-1.2.2
  % ./configure
  % make
  % make install

== The next step

Installation is completed. We should try
((<tutorial|TUTORIAL>)) with the installed Cutter.
