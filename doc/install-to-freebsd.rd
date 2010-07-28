# -*- rd -*-

= Install to FreeBSD --- How to install Cutter to FreeBSD

== Introduction

This document explains how to install Cutter to FreeBSD.

== Install

Cutter can be installed either using packages or ports. As usual
you shall not mix them so depending on the way you maintain third
party packages, install the package:

  % sudo /usr/sbin/pkg_add -r cutter

or compile and install the port:

  % (cd /usr/ports/devel/cutter && sudo make install)

See also:

  * about packages and ports:
    ((<Installing Applications: Packages and Ports|URL:http://www.freebsd.org/doc/en/books/handbook/ports.html>))
  * about the port:
    ((<"FreshPorts -- devel/cutter"|URL:http://www.freshports.org/devel/cutter/>))

== The next step

Installation is completed. We should try
((<tutorial|TUTORIAL>)) with the installed Cutter.
