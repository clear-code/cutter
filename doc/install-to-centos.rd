# -*- rd -*-

= Install to CentOS --- How to install Cutter to CentOS

== Introduction

This document explains how to install Cutter to CentOS.

== Install

We can use yum because Cutter provides a Yum repository.

First, we need to register Cutter Yum repository:

  % sudo rpm -Uvh http://cutter.sourceforge.net/centos/cutter-repository-1.0.0-1.noarch.rpm

Now, we can install Cutter by yum:

  % sudo yum install -y cutter

== The next step

Installation is completed. We should try
((<tutorial|TUTORIAL>)) with the installed Cutter.
