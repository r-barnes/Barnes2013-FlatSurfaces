Barnes2013-FlatSurfaces
=======================

**Title of Manuscript**:
An Efficient Assignment of Drainage Direction Over Flat
Surfaces in Raster Digital Elevation Models

**Authors**: Richard Barnes, Clarence Lehman, David Mulla

**Corresponding Author**: Richard Barnes (rbarnes@umn.edu)

**DOI Number of Manuscript**
[10.1016/j.cageo.2013.01.009](http://dx.doi.org/10.1016/j.cageo.2013.01.009)

**Code Repositories**
 * [Author's GitHub Repository](https://github.com/r-barnes/Barnes2013-FlatSurfaces)
 * [Journal's GitHub Repository](https://github.com/cageo)



Using This As A Tool
====================

Don't. If you want to use these algorithms as an out-of-the-box terrain analysis
system, please download [RichDEM](https://github.com/r-barnes/richdem).



The Source Code
===============
This repo references the RichDEM terrain analysis softare, of which these
algorithms are all a part. `main.cpp` will run all of the algorithms mentioned
above. The `#include` directives in `main.cpp` identify the necessary RichDEM
libraries for using these implementations for your own work.



Compilation
===========

After cloning this repo you must acquire RichDEM by running:

    git submodule init
    git submodule update

To compile the programs run:

    make

The result is a program called `flats.exe`.

The program is run by typing:

    ./flats.exe gentest <SIZE> <OUTPUT>
    ./flats.exe gentest 1000 test_dem.tif

    ./flats.exe <ALGORITHM> <INPUT> <OUTPUT>
    ./flats.exe 1 test_dem.tif barnes_out.tif
    ./flats.exe 2 test_dem.tif garbrecht_out.tif

The algorithms available are described briefly below and in greater detail in
the manuscript.



Input
======

This program reads in a digital elevation model (DEM) file specified on the
command line. The file may be of any type recognised by
[GDAL](http://www.gdal.org/). The program will run one of the algorithms
described in the manuscript (and below), store the result in an output file, and
report how long this took.



Output
======

The two reference implementations output files in the D8 neighbour system to
indicate flow directions. In this system all the flow from a central cell is
directed to a single neighbour which is represented by a number according to the
following system, where 0 indicates the central cell.

    234
    105
    876

The output is in GeoTIFF format.



The Algorithms
==============

This repository contains a reference implementation of the algorithms presented
in the manuscript above. These implementations were used in performing speed
comparison tests in the manuscript. The algorithms include

 * **Algorithm 1: Barnes et al. (2014)** The new algorithm presented in the
above manuscript.

 * **Algorithm 2: Garbrecht and Martz (1997)** The eponymous algorithm against
which the new algorithm is compared.




Generating Test Data
====================

You can generate a test dataset which emphasises the speed differences between
the two algorithms by running:

    ./flats.exe gentest <SIZE> test_dem.tif

This will generate a square DEM with a single outlet near the bottom-left
corner.



Notes
=====

Our implementation of Garbrecht and Martz (1997) attempts to reproduce the
eponymous algorithm from the description in the eponymous paper. Note that this
implementation does not apply itself iteratively, meaning that some flats will
be unresolvable. The efficiency differences of even a single iteration of G&M
(1997) are sufficient to support the argument in our manuscript.

The algorithms presented herein are part of the RichDEM code base, a collection
of state of the art algorithms for quick terrain analysis. RichDEM is available
at: [https://github.com/r-barnes/richdem](https://github.com/r-barnes/richdem).



Suggested Test Procedure
========================

You run tests similar to those discussed in the manuscript like so:

    ./flats.exe gentest 1000 test_dem.tif
    ./flats.exe 1 test_dem.tif barnes_out.tif
    ./flats.exe 2 test_dem.tif garbrecht_out.tif



Updates
=======================

Commit **131e21ef47ed4f97** (02016-09-14) and subsequent commits updated this
repo to rely explicitly on the RichDEM codebase. This broadened the set of
input/output formats available and synchronized the repo with the many benfits
and standardizations introduced to RichDEM since this code was originally
crafted.

The old codebase had the advantage of not relying on external libraries and
being readily accessible to all parties. It had the disadvantage of being a
slow, clumsy, and limited way to work with the data. The code now requires the
use of the GDAL library, greatly expanding the data formats and data types which
can be worked with, as well as greatly speeding up I/O.