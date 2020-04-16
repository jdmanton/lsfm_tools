# LSFM tools

A collection of tools for processing light sheet fluorescence microscopy (LSFM) data, particularly stage-scan data, written by [James Manton](https://github.com/jdmanton) and [Jérôme Boulanger](https://github.com/jboulanger).
Portable methods are provided for deskewing, deconvolving and projecting volumetric data at high speed without GPU-acceleration.


## Code organization

A single binary lsfm allows to call the various tools of the package.

An API (lsfm.h) exposes the main components so they can be called from other programs. The main components are split over several .cpp files to speed up compilation.

## Modules
### Deskewing
### Deconvolution
### Deskewing and maximum intensity projection
