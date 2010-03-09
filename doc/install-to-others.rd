# -*- rd -*-

= Install to other platform --- How to install Cutter to other platform

== Introduction

This document explains how to install Cutter to platforms
except ((<"Debian GNU/Linux"|install-to-debian.rd>)),
((<Ubuntu Linux|install-to-ubuntu.rd>)),
((<FreeBSD|install-to-freebsd.rd>)),
((<Solaris|install-to-solaris.rd>)) and
((<Cygwin|install-to-cygwin.rd>)).

== Install GLib

Cutter requires GLib 2.16 or later. We can install GLib that
satisfies the condition by ourselves and use it if our
system doesn't provide GLib that satisfies the
condition. (e.g. CentOS)

Here is an instruction to install GLib into ~/local/:

  % mkdir -p ~/src
  % cd ~/src
  % wget http://ftp.gnome.org/pub/GNOME/sources/glib/2.20/glib-2.20.1.tar.gz
  % tar xvfz glib-2.20.1.tar.gz
  % cd glib-2.20.1
  % ./configure --prefix=$HOME/local
  % make
  % make install

== Install Cutter

Cutter can be installed like other free software because
Cutter uses GNU build tools:

  % mkdir -p ~/src
  % cd ~/src
  % wget http://downloads.sourceforge.net/cutter/cutter-1.1.1.tar.gz
  % tar xvzf cutter-1.1.1.tar.gz
  % cd cutter-1.1.1
  % ./configure
  % make
  % sudo make install

Here is an instruction to use GLib installed into ~/local/
by ourselves. In the instruction, Cutter is also installed
into ~/local/:

  % mkdir -p ~/src
  % cd ~/src
  % wget http://downloads.sourceforge.net/cutter/cutter-1.1.1.tar.gz
  % tar xvzf cutter-1.1.1.tar.gz
  % cd cutter-1.1.1
  % ./configure PKG_CONFIG_PATH=$HOME/local/lib/pkgconfig LD_LIBRARY_PATH=$HOME/local/lib --prefix=$HOME/local
  % make
  % make install

== The next step

Installation is completed. We should try
((<tutorial|TUTORIAL>)) with the installed Cutter.
