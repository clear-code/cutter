# -*- rd -*-

= Install to Fedora --- How to install Cutter to Fedora

== Introduction

This document explains how to install Cutter to Fedora.

== Install dependency libraries

Here is a dependency library to build Cutter:

  * GLib

Here are dependency libraries needed by development with
Cutter:

  * autoconf
  * automake
  * libtool
  * intltool

Those packages can be installed with yum:

  % sudo yum install -y glib2 automake libtool intltool

== Install Cutter

There is a RPM package for Cutter on Fedora 10 amd64. If you
need a package for another environment, you can request on
((<mailing
list|URL:https://lists.sourceforge.net/lists/listinfo/cutter-users-en>)).

  % wget http://downloads.sourceforge.net/cutter/cutter-1.1.0-0.x86_64.rpm
  % sudo rpm -Uvh cutter-1.1.0-0.x86_64.rpm

== The next step

Installation is completed. We should try
((<tutorial|TUTORIAL>)) with the installed Cutter.
