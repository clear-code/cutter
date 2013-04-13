# -*- rd -*-

= Install to Ubuntu --- How to install Cutter to Ubuntu

== Introduction

This document explains how to install Cutter to Ubuntu
Linux.

== Install

We can use aptitude because Cutter provides Debian packages.

There are packages for Ubuntu Lucid Lynx/Natty Narwhal/Oneiric Ocelot/
Precise Pangloin i386/amd64.
If you need a package for another environment,
you can request on ((<mailing
list|URL:https://lists.sourceforge.net/lists/listinfo/cutter-users-en>)).

Here are apt lines for for Lucid. We put them into
/etc/apt/sources.list.d/cutter.list.

/etc/apt/sources.list.d/cutter.list:
  deb http://downloads.sourceforge.net/project/cutter/ubuntu/ lucid main
  deb-src http://downloads.sourceforge.net/project/cutter/ubuntu/ lucid main

Here are apt lines for for Natty. We put them into
/etc/apt/sources.list.d/cutter.list.

/etc/apt/sources.list.d/cutter.list:
  deb http://downloads.sourceforge.net/project/cutter/ubuntu/ natty main
  deb-src http://downloads.sourceforge.net/project/cutter/ubuntu/ natty main

Here are apt lines for for Oneiric. We put them into
/etc/apt/sources.list.d/cutter.list.

/etc/apt/sources.list.d/cutter.list:
  deb http://downloads.sourceforge.net/project/cutter/ubuntu/ oneiric main
  deb-src http://downloads.sourceforge.net/project/cutter/ubuntu/ oneiric main

Here are apt lines for for Precise. We put them into
/etc/apt/sources.list.d/cutter.list.

/etc/apt/sources.list.d/cutter.list:
  deb http://downloads.sourceforge.net/project/cutter/ubuntu/ precise main
  deb-src http://downloads.sourceforge.net/project/cutter/ubuntu/ precise main

Here are apt lines for for Quantal. We put them into
/etc/apt/sources.list.d/cutter.list.

/etc/apt/sources.list.d/cutter.list:
  deb http://downloads.sourceforge.net/project/cutter/ubuntu/ quantal main
  deb-src http://downloads.sourceforge.net/project/cutter/ubuntu/ quantal main

Cutter packages are signed by key of cutter-keyring.
Register the key by installing cutter-keyring package.

  % sudo apt-get update
  % sudo apt-get -y --allow-unauthenticated install cutter-keyring

If you install the keyring package, you can install Cutter by apt-get:

  % sudo apt-get update
  % sudo apt-get -y install cutter-testing-framework

== The next step

Installation is completed. We should try
((<tutorial|TUTORIAL>)) with the installed Cutter.
