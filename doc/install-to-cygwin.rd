# -*- rd -*-

= Install to Cygwin --- How to install Cutter to Cygwin

== Introduction

This document explains how to install Cutter to Cygwin.

You can install Cutter to Cygwin 1.7.

== Install Cygwin

First, download Cygwin 1.7 installer:

  * ((<Installer for 32-bit environment|URL:http://cygwin.com/setup-x86.exe>))
  * ((<Installer for 64-bit environment|URL:http://cygwin.com/setup-x86_64.exe>))

Next, install the following packages with the downloaded
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

Enter into Cygwin. Now, you can run Cygwin.

== Install Cutter

Install Cutter:

  % cd
  % wget http://downloads.sourceforge.net/cutter/cutter-1.2.4.tar.gz
  % tar xvzf cutter-1.2.4.tar.gz
  % cd cutter-1.2.4
  % ./configure
  % make
  % make install

== The next step

Installation is completed. You should try
((<tutorial|TUTORIAL>)) with the installed Cutter.
