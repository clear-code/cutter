# -*- rd -*-

= Install to Debian GNU/Linux --- How to install Cutter to Debian GNU/Linux

== Introduction

This document explains how to install Cutter to Debian
GNU/Linux. We can use the same steps for Ubuntu.

== Install

We can use aptitude because Cutter provides Debian packages.

We need to add the following apt lines to
/etc/apt/sources.list:

  deb http://cutter.sourceforge.net/debian/ unstable main
  deb-src http://cutter.sourceforge.net/debian/ unstable main

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
