# Installing gmpy2 on Windows #

Pre-compiled versions of **gmpy2** are available on the [Downloads](http://code.google.com/p/gmpy/downloads/list) page. Please select the installer that corresponds to the version of Python you have installed on your computer. Note that either a 32 or 64-bit version of Python can be installed on a 64-bit version of Windows. If you get an error message stating that Windows could not be found in the registry, you have the wrong version of the installer.

If you want to compile **gmpy2** yourself, please consult the file "vs2010\_build.txt" included with the source code.

# Installing gmpy2 on Linux #

Many Linux distributions provide **gmpy2** in their respective repositories. Please check the repositories first.

## Requirements ##

If you want to compile a more recent version of **gmpy2** than provided by your distribution, or if your distribution does not provide **gmpy2**, you will need to compile **gmpy2** from source.

**gmpy2** has only been tested with recent versions of GMP (or MPIR) and (optionally) MPFR. Specifically, for integer and rational support, **gmpy2** requires either GMP 5.0.x or later, or MPIR 2.6.0 or later. To support multiple precision floating point arithmetic, MPFR 3.1.0 or later is required. To support multiple precision complex arithmetic, MPC version 1.0 or later is required.

## Short Instructions ##

You will need to install the required development libraries. Installing the **libmpc-dev** (or similarly named) package should install the required files. Then compiling **gmpy2** should be as simple as:

```
cd <gmpy2 source directory>
python setup.py install
```

If this fails, read on.

## Detailed Instructions ##


---


### The following instructions are for **gmpy2** version 2.0.6 or later! ###

### If you are compiling an earlier source version, please replace the existing setup.py with https://code.google.com/p/gmpy/source/browse/tags/gmpy2_maint/setup.py. ###


---


If your Linux distribution does not support recent versions of GMP, MPFR, and MPC, you will need to compile your own versions. To avoid any possible conflict with existing libraries on your system, the following instructions install GMP, MPFR, and MPC to a location in your home directory. The **gmpy2** extension is also statically linked for easier installation.

### Compiling GMP, MPFR, and MPC ###

  1. Create the desired destination directory for the GMP, MPFR, and MPC library files. The following instructions use $HOME/static for the GMP, MPFR, and MPC static libraries. The instructions also assume the source for GMP, MPFR, and MPC is located in $HOME/src.
```
$ mkdir $HOME/static
```
  1. Download and un-tar the GMP source code. Change to GMP source directory and compile GMP.
```
$ cd $HOME/src/gmp-6.0.0
$ ./configure --prefix=$HOME/static --enable-static --disable-shared --with-pic --enable-fat
$ make
$ make check
$ make install
```
  1. Download and un-tar the MPFR source code. Change to MPFR source directory and compile MPFR.
```
$ cd $HOME/src/mpfr-3.1.2
$ ./configure --prefix=$HOME/static --enable-static --disable-shared --with-pic --with-gmp=$HOME/static
$ make
$ make check
$ make install
```
  1. Download and un-tar the MPC source code. Change to MPC source directory and compile MPC.
```
$ cd $HOME/src/mpc-1.0.1
$ ./configure --prefix=$HOME/static --enable-static --disable-shared --with-pic --with-gmp=$HOME/static --with-mpfr=$HOME/static
$ make
$ make check
$ make install
```
  1. Compile **gmpy2** and specify the location of GMP, MPFR, and MPC.
```
$ python setup.py build_ext --static=$HOME/static install
```
  1. If you get a "permission denied" error message, you may need to use
```
$ python setup.py build_ext --static=$HOME/static
$ sudo python setup.py install
```