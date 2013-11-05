# -*- rd -*-

= Travis CI integration

== Introduction

This section describes about using Cutter on ((<Travis CI|URL:http://travis-ci.org/>)).
Travis CI is a hosted continuous integration service for the open source community.

You can use Travis CI for your open source software. This section only
describes about Cutter related configuration.
See ((<"Travis CI:Documentation"|URL:http://about.travis-ci.org/docs/>)) 
about general Travis CI.

== Configuration

Travis CI is running on 64-bit Ubuntu 12.04. (See ((<Travis CI: About
Travis CI Environment|URL:http://about.travis-ci.org/docs/user/ci-environment/>)).)
You can use apt-line for Ubuntu 12.04 provided by Cutter project to install
Cutter on Travis CI.

You can custom build lifecycle by (({.travis.yml})). (See ((<Travis CI:
Conifugration your Travis CI build with .travis.yml
|URL:http://about.travis-ci.org/docs/user/build-configuration/>)).) You
can use (({before_install:})) hook or (({install:})) hook.

=== Configuration for C/C++

You should execute the setup script at (({install:})) hook
because there is no dominant convention in the community about dependency management,
Travis CI skips dependency installation for C/C++ projects. ((- 
((<"Travis CI: Building a C Project"|URL:http://about.travis-ci.org/docs/user/languages/c/>))
'Dependency Management' or
((<"Travis CI: Building a C Project"|URL:http://about.travis-ci.org/docs/user/languages/cpp/>))
'Dependency Management' section explains that Travis CI does not install specific
dependency installation. -))

Add the following (({install:})) configuration to (({.travis.yml})):

  install:
    - curl https://raw.github.com/clear-code/cutter/master/data/travis/setup.sh | sh

With the above configuration, you can use Cutter for your build.

=== Configuration for other languages

You should execute the setup script at (({before_install:})) hook 
not to override default dependency management rules 
which depends on each project's language if you are writing language binding
for specific C/C++ library and executing its tests by Cutter at your project. ((- 
((<"Travis CI: Configuring your Travis CI build with 
.travis.yml"|URL:http://about.travis-ci.org/docs/user/build-configuration/>))
'Build Lifecycle' section explains that Travis CI executes languange specific
dependency installation at (({install:})) hook. -))

Add the following (({before_install:})) configuration to (({.travis.yml})):

  before_install:
    - curl https://raw.github.com/clear-code/cutter/master/data/travis/setup.sh | sh

With the above configuration, you can use Cutter for your build.

== Examples

Here is free software that use Cutter on Travis CI:

  * ((<groonga|URL:http://groonga.org/>))

    * ((<groonga on Travis CI|URL:http://travis-ci.org/#!/groonga/groonga>))
    * ((<.travis.yml for groonga|URL:https://github.com/groonga/groonga/blob/master/.travis.yml>))

