# -*- rd -*-

= Install to Ubuntu Linux --- How to install Cutter to Ubuntu Linux

== Introduction

This document explains how to install Cutter to Ubuntu
Linux.

== Install

We can use aptitude because Cutter provides Debian packages.

There are packages for Ubuntu Linux Hardy Heron/Jaunty
Jackalope i386/amd64. If you need a package for another
environment, you can request on ((<mailing
list|URL:https://lists.sourceforge.net/lists/listinfo/cutter-users-en>)).

Here are apt lines for for Hardy. We put them into
/etc/apt/sources.list/cutter.list.

/etc/apt/sources.list/cutter.list:
  deb http://cutter.sourceforge.net/ubuntu/ hardy main
  deb-src http://cutter.sourceforge.net/ubuntu/ hardy main

Here are apt lines for for Jaunty. We put them into
/etc/apt/sources.list/cutter.list.

/etc/apt/sources.list/cutter.list:
  deb http://cutter.sourceforge.net/ubuntu/ jaunty main
  deb-src http://cutter.sourceforge.net/ubuntu/ jaunty main

Cutter packages are signed by key of
kou@cozmixng.org/kou@clear-code.com. If we trust the key, we
can register the key:

  % gpg --keyserver hkp://subkeys.pgp.net --recv-keys 1C837F31
  % gpg --export 1C837F31 | sudo apt-key add -

If we register the key, we can install Cutter by aptitude:

  % sudo aptitude update
  % sudo aptitude -V -D install cutter-testing-framework

== The next step

Installation is completed. We should try
((<tutorial|TUTORIAL>)) with the installed Cutter.
