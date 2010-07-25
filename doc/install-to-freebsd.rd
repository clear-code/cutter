# -*- rd -*-

= Install to FreeBSD --- How to install Cutter to FreeBSD

== Introduction

This document explains how to install Cutter to FreeBSD.

== Install

Cutter can be installed either using packages or ports. As usual
you shall not mix them so depending on the way you maintain third
party packages, install the package:

  % sudo /usr/local/sbin/portupgrade -PNRr cutter

or compile and install the port:

  % sudo /usr/local/sbin/portupgrade -NRr cutter

== The next step

Installation is completed. We should try
((<tutorial|TUTORIAL>)) with the installed Cutter.
