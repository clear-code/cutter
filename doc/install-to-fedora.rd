# -*- rd -*-

= Install to Fedora --- How to install Cutter to Fedora

== Introduction

This document explains how to install Cutter to Fedora.

== Install

We can use yum because Cutter provides a Yum repository.

First, we need to register Cutter Yum repository:

  % sudo rpm -Uvh http://downloads.sourceforge.net/project/cutter/fedora/cutter-release-1.1.0-0.noarch.rpm

Now, we can install Cutter by yum:

  % sudo yum install -y cutter-devel

Install cutter-gui package if you want to use GTK+ frontend.

  % sudo yum install -y cutter-gui

Install cutter-report package if you want to save testing results as PDF format.

  % sudo yum install -y cutter-report

== The next step

Installation is completed. We should try
((<tutorial|TUTORIAL>)) with the installed Cutter.
