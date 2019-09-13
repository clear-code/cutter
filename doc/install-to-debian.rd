# -*- rd -*-

= Install to Debian GNU/Linux --- How to install Cutter to Debian GNU/Linux

== Introduction

This document explains how to install Cutter to Debian
GNU/Linux.

== Install

You can use apt-get because Cutter provides Debian packages.

There are packages for Debian GNU/Linux jessie/stretch
i386/amd64. If you need a package for another environment,
you can request on ((<mailing
list|URL:https://lists.sourceforge.net/lists/listinfo/cutter-users-en>)).

Here are apt lines for stretch. Put them into
/etc/apt/sources.list.d/cutter.list.

/etc/apt/sources.list.d/cutter.list:
  deb https://osdn.net/projects/cutter/storage/debian/ stretch main
  deb-src https://osdn.net/projects/cutter/storage/debian/ stretch main

Cutter packages are signed by key of cutter-keyring.
Register the key by installing cutter-keyring package.

  % sudo apt-get update --allow-insecure-repositories
  % sudo apt-get -y --allow-unauthenticated install cutter-keyring

If you install the keyring package, you can install Cutter by apt-get:

  % sudo apt-get update
  % sudo apt-get -y install cutter-testing-framework

== The next step

Installation is completed. You should try ((<tutorial|TUTORIAL>)) with
the installed Cutter.
