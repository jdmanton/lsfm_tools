
#depends on libtiff-dev libfftw3-dev

# to cross compile for windows using MXE use:
# make CROSS=x86_64-w64-mingw32.static- lsfm.exe

CXX=$(CROSS)g++

PKG = lsfm
VER = 0.1

CXXFLAGS = -Dcimg_use_cpp11 -Dcimg_use_openmp -Dcimg_use_tiff -Dcimg_use_fftw3 -std=c++11 -fopenmp -pthread -L./

EXE = lsfm

lsfm_SRCS = lsfm.cpp
lsfm_LIBS = lsfm tiff jpeg lzma z fftw3_omp fftw3 m X11

LIB = liblsfm.a

liblsfm.a_SRCS = deskew.cpp proj.cpp
liblsfm.a_LIBS = fftw3_omp fftw3 m

include magick.mk
