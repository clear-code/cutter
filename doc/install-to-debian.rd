# -*- rd -*-

= Install to Debian GNU/Linux --- How to install Cutter to Debian GNU/Linux

== Introduction

This document explains how to install Cutter to Debian
GNU/Linux.

== Install

You can use apt-get because Cutter provides Debian packages.

There are packages for Debian GNU/Linux stretch/buster 
i386/amd64. If you need a package for another environment,
you can request on ((<mailing
list|URL:https://lists.osdn.me/mailman/listinfo/cutter-users-en>)).

Here are apt lines for stretch. Put them into
/etc/apt/sources.list.d/cutter.list.

/etc/apt/sources.list.d/cutter.list:
  deb [signed-by=/usr/share/keyrings/cutter-keyring.gpg] https://osdn.net/projects/cutter/storage/debian/ stretch main
  deb-src [signed-by=/usr/share/keyrings/cutter-keyring.gpg] https://osdn.net/projects/cutter/storage/debian/ stretch main

  % sudo wget -O /usr/share/keyrings/cutter-keyring.gpg https://osdn.net/projects/cutter/storage/debian/cutter-keyring.gpg
  % sudo apt-get update
  % sudo apt-get -y install cutter-testing-framework

Here are apt lines for buster. Put them into
/etc/apt/sources.list.d/cutter.list.

/etc/apt/sources.list.d/cutter.list:
  deb [signed-by=/usr/share/keyrings/cutter-keyring.gpg] https://cutter.osdn.jp/debian/ buster main
  deb-src [signed-by=/usr/share/keyrings/cutter-keyring.gpg] https://cutter.osdn.jp/debian/ buster main

  % sudo wget -O /usr/share/keyrings/cutter-keyring.gpg https://cutter.osdn.jp/debian/cutter-keyring.gpg
  % sudo apt-get update
  % sudo apt-get -y install cutter-testing-framework

== The next step

Installation is completed. You should try ((<tutorial|TUTORIAL>)) with
the installed Cutter.
