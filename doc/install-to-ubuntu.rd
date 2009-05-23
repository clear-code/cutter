# -*- rd -*-

= Install to Ubuntu Linux --- How to install Cutter to Ubuntu Linux

== Introduction

This document explains how to install Cutter to Ubuntu
Linux.

== Install

We can use aptitude because Cutter provides Debian packages.

There is a package for Ubuntu Linux Hardy Heron amd64. If
you need a package for another environment, you can request
on ((<mailing
list|URL:https://lists.sourceforge.net/lists/listinfo/cutter-users-en>)).

We need to add the following apt lines to
/etc/apt/sources.list:

  deb http://cutter.sourceforge.nef/ubuntu/ hardy main
  deb-src http://cutter.sourceforge.net/ubuntu/ hardy main

Cutter packages are signed by key of
kou@cozmixng.org/kou@clear-code.com If we trust the key, we
register the key:

  % gpg --keyserver hkp://subkeys.pgp.net --recv-keys 1C837F31
  % gpg --export 1C837F31 | sudo apt-key add -

If we register the key, we can install Cutter by aptitude:

  % sudo aptitude update
  % sudo aptitude -V -D install cutter-testing-framework

== The next step

Installation is completed. We should try
((<tutorial|TUTORIAL>)) with the installed Cutter.
