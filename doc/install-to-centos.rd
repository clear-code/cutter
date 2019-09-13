# -*- rd -*-

= Install to CentOS --- How to install Cutter to CentOS

== Introduction

This document explains how to install Cutter to CentOS.

== Install

We can use yum because Cutter provides a Yum repository.

First, we need to register Cutter Yum repository.

CentOS 6:

  % sudo yum install -y https://osdn.net/projects/cutter/storage/centos/cutter-release-1.3.0-1.noarch.rpm

CentOS 7:

  % sudo yum install -y https://osdn.net/projects/cutter/storage/centos/cutter-release-1.3.0-1.noarch.rpm

Now, we can install Cutter by yum:

  % sudo yum install -y cutter

== The next step

Installation is completed. We should try
((<tutorial|TUTORIAL>)) with the installed Cutter.
