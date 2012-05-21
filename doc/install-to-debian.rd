# -*- rd -*-

= Install to Debian GNU/Linux --- How to install Cutter to Debian GNU/Linux

== Introduction

This document explains how to install Cutter to Debian
GNU/Linux.

== Install

We can use aptitude because Cutter provides Debian packages.

There are packages for Debian GNU/Linux squeeze/wheezy/sid
i386/amd64. If you need a package for another environment,
you can request on ((<mailing
list|URL:https://lists.sourceforge.net/lists/listinfo/cutter-users-en>)).

Here are apt lines for for squeeze. We put them into
/etc/apt/sources.list.d/cutter.list.

/etc/apt/sources.list.d/cutter.list:
  deb http://downloads.sourceforge.net/project/cutter/debian/ squeeze main
  deb-src http://downloads.sourceforge.net/project/cutter/debian/ squeeze main

Here are apt lines for for wheezy. We put them into
/etc/apt/sources.list.d/cutter.list.

/etc/apt/sources.list.d/cutter.list:
  deb http://downloads.sourceforge.net/project/cutter/debian/ wheezy main
  deb-src http://downloads.sourceforge.net/project/cutter/debian/ wheezy main

Here are apt lines for for sid. We put them into
/etc/apt/sources.list.d/cutter.list.

/etc/apt/sources.list.d/cutter.list:
  deb http://downloads.sourceforge.net/project/cutter/debian/ unstable main
  deb-src http://downloads.sourceforge.net/project/cutter/debian/ unstable main

Cutter packages are signed by key of
kou@cozmixng.org/kou@clear-code.com. If we trust the key, we
can register the key:

  % sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 1C837F31

If we register the key, we can install Cutter by aptitude:

  % sudo aptitude update
  % sudo aptitude -V -D install cutter-testing-framework

== The next step

Installation is completed. We should try
((<tutorial|TUTORIAL>)) with the installed Cutter.
