# -*- rd -*-

= Travis CI integration

== Introduction

This section describes about using Cutter on ((<Travis CI|URL:http://travis-ci.org/>)).
Travis CI is a hosted continuous integration service for the open source community.

You can use Travis CI for your open source software. This section only
describes about groonga related configuration.
See ((<"Travis CI:Documentation"|URL:http://about.travis-ci.org/docs/>)) 
about general Travis CI.

== Configuration

Travis CI is running on 32-bit Ubuntu 11.10. (See ((<Travis CI: About
Travis CI Environment|URL:http://about.travis-ci.org/docs/user/ci-environment/>)).)
You can use apt-line for Ubuntu 11.10 provided by Cutter project to install
Cutter on Travis CI.

You can custom build lifecycle by (({.travis.yml})). (See ((<Travis CI:
Conifugration your Travis CI build with .travis.yml
|URL:http://about.travis-ci.org/docs/user/build-configuration/>)).) You
can use (({before_install})) hook or (({install})) hook. You should use
(({before_install})) if your software uses a language that is supported
by Travis CI such as Ruby. You should use (({install})) otherwise.

Add the following (({before_install})) configuration to (({.travisyml}))::

  before_install:
    - curl https://raw.github.com/clear-code/cutter/master/data/travis/setup.sh | sh

If you need to use (({install})) hook instead of (({before_install})), you
just substitute (({before_install:})) with (({install:})).

With the above configuration, you can use Cutter for your build.

== Examples

Here are open source softwares that use Cutter on Travis CI:

  * ((<groonga|URL:http://groonga.org/>))

    * ((<groonga on Travis CI|URL:http://travis-ci.org/#!/groonga/groonga>))
    * ((<.travis.yml for groonga|URL:https://github.com/groonga/groonga/blob/master/.travis.yml>))

