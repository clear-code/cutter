# -*- rd -*-

= Install to CentOS --- How to install Cutter to CentOS

== Introduction

This document explains how to install Cutter to CentOS.

== Install

We can use yum because Cutter provides a Yum repository.

First, we need to register Cutter Yum repository.
Note that you must download RPM package on CentOS 5.

CentOS 5:

  % wget http://downloads.sourceforge.net/project/cutter/centos/cutter-release-1.1.0-0.noarch.rpm
  % rpm -Uvh cutter-release-1.1.0-0.noarch.rpm


CentOS 6:

  % sudo rpm -Uvh http://downloads.sourceforge.net/project/cutter/centos/cutter-release-1.1.0-0.noarch.rpm

Now, we can install Cutter by yum:

  % sudo yum install -y cutter

== The next step

Installation is completed. We should try
((<tutorial|TUTORIAL>)) with the installed Cutter.
