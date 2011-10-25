# -*- rd -*-

= Install to Mac OS X --- How to install Cutter to Mac OS X

== Introduction

This document explains how to install Cutter to Mac OS X.

== Install

We can use port because Cutter provides MacPorts.

First, we get MacPorts for Cutter:

  % cd ~
  % git clone https://github.com/clear-code/cutter-macports.git

Next, we register MacPorts for Cutter:

  % echo file://$HOME/cutter-macports | sudo sh -c "cat >> /opt/local/etc/macports/sources.conf"

We can install Cutter by port:

  % sudo port install cutter

== The next step

Installation is completed. We should try
((<tutorial|TUTORIAL>)) with the installed Cutter.
