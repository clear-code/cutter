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

Cutter packages are signed by key of cutter-keyring.
Register the key by installing cutter-keyring package.

  % sudo aptitude update
  % sudo aptitude -V -D -y --allow-untrusted install cutter-keyring

If you install the keyring package, you can install Cutter by aptitude:

  % sudo aptitude update
  % sudo aptitude -V -D -y install cutter-testing-framework

== The next step

Installation is completed. We should try
((<tutorial|TUTORIAL>)) with the installed Cutter.
