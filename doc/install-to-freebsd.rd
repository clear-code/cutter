# -*- rd -*-

= Install to FreeBSD --- How to install Cutter to FreeBSD

== Introduction

This document explains how to install Cutter to FreeBSD.

== Install dependency libraries

Here is a dependency library to build Cutter:

  * GLib

Here are dependency libraries needed by development with
Cutter:

  * autoconf
  * automake
  * libtool
  * intltool

Those packages can be installed from ports with portupgrade:

  % sudo /usr/local/sbin/portupgrade -NRr devel/glib20 autotools intltool

== Install Cutter

Cutter can be installed like other free software because
Cutter uses GNU build tools:

  % mkdir -p ~/src
  % cd ~/src
  % fetch http://downloads.sourceforge.net/cutter/cutter-1.1.2.tar.gz
  % tar xvzf cutter-1.1.2.tar.gz
  % cd cutter-1.1.2
  % ./configure CPPFLAGS="-I/usr/local/include"
  % gmake
  % sudo gmake install

== The next step

Installation is completed. We should try
((<tutorial|TUTORIAL>)) with the installed Cutter.
