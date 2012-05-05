# Huh?

jiss - Jack Interactive Sequencer Software

# Dependencies

* lua5.1
* liblua5.1-0-dev
* swig
* swig-lua
* boost-shared-pointer
* boost-function
* libjack-dev
* lua-md5


# How to build?

> make

> sudo make install

# How to run?

> lua -i examples/test.lua

The -i switch causes lua to keep running after executing the script. This is what you want most of the time

Or you can also open an interactive session via

> lua

Then do some stuff:

> require "jiss"

> -- do your funky stuff here

> -- e.g. load test.lua and work with it

> require "test"

# Documentation?

This is still a prototype, so don't expect anything :D Read the code. It's small.. Do checkout the wiki though:

[https://github.com/fps/jiss/wiki](https://github.com/fps/jiss/wiki)

You can also generate some docs from the source running 

> make doc

This will create documentation in the doc/ folder

# Author

Florian Paul Schmidt (mista.tapas at gmx dot net)

