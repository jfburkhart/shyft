|                        |                                                                                                                                                     |
|------------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------|
| Shyft Documentation    | [![Documentation Status](https://readthedocs.org/projects/shyft/badge/?version=latest)](http://shyft.readthedocs.org/en/latest/)                    |
| Travis Build           | [![Build Status](https://travis-ci.org/statkraft/shyft.svg?branch=master)](https://travis-ci.org/statkraft/shyft)                                   |
| Shyft Google Group     | [![Shyft Google Group](https://img.shields.io/badge/Shyft%20Google%20Group-active-blue.svg)](https://groups.google.com/forum/#!forum/shyft)         |
| License                | [![Gnu GPL license](https://img.shields.io/badge/license-GPLv2-blue.svg)](https://github.com/statkraft/shyft/blob/master/LICENSE)                   |


# ABOUT

Shyft is an open source hydrological toolbox developed at [Statkraft](http://www.statkraft.com). It is optimized for highly efficient modeling of hydrologic processes following the paradigm of distributed, lumped parameter models -- with recent developments introducing more physically based / process-level methods.


# DOCUMENTATION

Shyft's primary **end-user** documentation is at [Shyft readthedocs](http://shyft.readthedocs.io/en/latest/), where you will find instructions for installing Shyft and getting up and running with the tools it provides.

We also maintain this [README](README.md) file with basic instructions for building Shyft from a **developer** perspective.

*IMPORTANT*: While Shyft is being developed to support Linux and Windows platforms, it should be noted that the instructions contained in this README are geared toward linux systems. Users will generally want to [readthedocs](http://shyft.readthedocs.io/en/latest/) first.


# AUTHORS

Shyft is developed by Statkraft, and the two main initial authors to
the C++ core were Sigbjørn Helset <Sigbjorn.Helset@statkraft.com> and
Ola Skavhaug <ola@xal.no>.

Orchestration and the Python wrappers were originally developed by
John F. Burkhart <john.burkhart@statkraft.com> with later contributions
from Yisak Sultan Abdella <yisaksultan.abdella@statkraft.com>

Copyright (C) Sigbjørn Helset (SiH), John F. Burkhart (JFB), Ola Skavhaug (OS), Yisak Sultan Abdella (YAS), Statkraft AS


# THANKS

Contributors and current project participants include:
 * Sigbjørn Helset <Sigbjorn.Helset@statkraft.com>
 * Ola Skavhaug <ola@xal.no>
 * John Burkhart <John.Burkhart@statkraft.com>
 * Yisak Sultan Abdella <YisakSultan.Abdella@statkraft.com>
 * Felix Matt <f.n.matt@geo.uio.no>
 * Francesc Alted <faltet@gmail.com>
 

# COPYING / LICENSE

Shyft is released under LGPL V.3
See LICENCE


## DEVELOPER DOCUMENTATION

The documentation below is maintained for the purposes of Shyft development. First time users and those are interested in learning how to use Shyft for hydrologic simulation are strongly encouraged to see [Shyft at readthedocs](http://shyft.readthedocs.io/en/latest/).



### CLONING

Shyft is distributed in three separate code repositories. This repository, `shyft` provides the main code base. A second repository (required for tests) is located at [shyft-data](https://github.com/statkraft/shyft-data). A third repository [shyft-doc](https://github.com/statkraft/shyft-doc) is available containing example notebooks and tutorials. The three repositories assume they have been checked out in parallel into a `shyft_workspace` directory:

```bash
mkdir shyft_workspace && cd shyft_workspace
export SHYFT_WORKSPACE=`pwd`
git clone https://github.com/statkraft/shyft.git
git clone https://github.com/statkraft/shyft-data.git
git clone https://github.com/statkraft/shyft-doc.git
```

### REQUIREMENTS


For compiling and running Shyft, you will need:

* A C++1y compiler (gcc-7 or higher)
* The BLAS and LAPACK libraries (development packages)
* A Python3 (3.6 or higher) interpreter
* The NumPy package (>= 1.8.0)
* The netCDF4 package (>= 1.2.1)
* The CMake building tool (3.9 or higher)
* 3rd party dependencies for c++ extensions and tests
  boost, dlib, armadillo, doctest

In addition, a series of Python packages are needed mainly for running the tests. These can be easily installed via:

```bash
$ pip install -r requirements.txt
```

or, if you are using conda (see below):

```bash
$ cat requirements.txt | xargs conda install
```


### PYTHON SET UP

Please refer to our [Python Installation Guide](http://shyft.readthedocs.io/en/latest/install_python.html#python-installation-guide)

### BUILDING


NOTE: the build/compile instructions below have been mainly tested on Linux platforms. Shyft can also be compiled (and it is actively maintained) for Windows, but the building instructions are not covered here (yet).

NOTE: the dependency regarding a modern compiler generally means gcc-7 is required to build Shyft.

You can compile Shyft by using the typical procedure for Python packages. 

Shyft currently uses boost, dlib, armadillo and doctest to build the python-extensions.

The dependencies can be provided as pr. standard on your linux-system, 
or 
built from source following standard build-recipe from those above mentioned libraries.

We supply scripts to automate the build-from source strategy: 

shyft/build_support/build_dependencies.sh  (linux)
shyft/build_support/win_build_dependencies.sh (windows)

You should execute the build_dependencies.sh script just after initial checkout or refresh,
prior to building the python extensions. The scripts will download and build required
packages in `shyft_dependencies` directory in parallel with shyft directory.

The linux build will also download miniconda with required packages for the shyft_env
in parallel with the shyft directory, effectively giving a complete sandboxed shyft
development setup.

You should then prepend to miniconda/bin to PATH prior to working with shyft
to ensure that the correct python interpreter is picked up.

When you call `setup.py` the script will call cmake. If the dependencies exist in the aforementioned directory, they will be used.
Otherwise cmake will attempt to locate the libraries from the system.

 
 ```bash
 pip install -r requirements.txt
 python setup.py build_ext --inplace 
 
 ```

NOTE: If you haven't set `env_vars` as part of your conda environment, then you need to do the following:

```bash
# assumes you are still in the shyft_workspace directory containing
# the git repositories
bash shyft/build_support/build_dependencies.sh
export PATH=$SHYFT_WORKSPACE/miniconda/bin:$PATH
export LD_LIBRARY_PATH=$SHYFT_WORKSPACE/shyft_dependencies/lib
cd shyft #the shyft repository
python setup.py build_ext --inplace
```


### QUICK TEST

It is recommended to at least run a few of the tests after building. This will ensure your paths and environment variables are set correctly.

The quickest and easiest test to run is:

```bash
python -c "from shyft import api"
```

To run further tests, see the TESTING section below. 

### INSTALLING

If the tests above run, then you can simply install Shyft using:

```bash
cd $SHYFT_WORKSPACE/shyft
python setup.py install
```

Now, you should be set to start working with the [shyft documentation](https://shyft.readthedocs.org) and 
ideally clone the [shyft-doc](https://github.com/statkraft/shyft-doc) repositories to work through the 
notebooks and learning Shyft!


### COMPILING MANUALLY VIA CMAKE


Although (at least on Linux) the `setup.py` method above uses the
CMake building tool behind the scenes, you can also compile it
manually (in fact, if you plan to develop Shyft, this may be recommended because you will be able to run
the integrated C++ tests).  The steps are the usual ones:

```bash
$ cd $SHYFT_WORKSPACE/shyft
$ mkdir build
$ cd build
$ cmake ..      # configuration step; or "ccmake .." for curses interface
$ make -j 4     # do the actual compilation of C++ sources (using 4 processes)
$ make install  # install python extensions into the shyft python source tree
```

We have the beast compiled by now.  For testing:

```bash
$ export LD_LIBRARY_PATH=$SHYFT_WORKSPACE/shyft_dependencies/lib
$ make test     # run the C++ tests
$ export PYTHONPATH=$SHYFT_WORKSPACE/shyft
$ nosetests ..  # run the Python tests
```

If all the tests pass, then you have an instance of Shyft that is
fully functional.  In case this directory is going to act as a
long-term installation it is recommended to persist your
`$LD_LIBRARY_PATH` and `$PYTHONPATH` environment variables (in `~/.bashrc`
or using the conda `env_vars` described above).


### TESTING


The way to test Shyft is by running:

```bash
$ nosetests
```
from the root shyft repository directory.

The test suite is comprehensive, and in addition to unit-tests covering c++ parts and python parts, it also covers integration tests with netcdf and geo-services.

Shyft tests are meant to be run from the sources directory. As a start, you can run the python api test suite by:

```bash
cd $SHYFT_WORKSPACE/shyft/shyft/tests/api
nosetests
```

### Comprehensive Tests

To conduct further testing and to run direct C++ tests, you need to be sure you have the `shyft-data` repository as a sibling of the `shyft` repository directory.

To run some of the C++ core tests you can try the following:

```bash
cd $SHYFT_WORKSPACE/shyft/build/test
make test
```
