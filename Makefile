all: deskew_mip deskew_linear
.PHONY: all

deskew_mip: deskew_mip.cpp
		$(CXX) -Dcimg_use_cpp11 -Dcimg_use_openmp -Dcimg_use_tiff -Dcimg_use_zlib \
		-std=c++11 -fopenmp -pthread -Ofast \
		-Wall -Wextra -Wfatal-errors -Werror=unknown-pragmas -Werror=unused-label \
		-I ./ deskew_mip.cpp -o deskew_mip \
		-lm -ltiff -ljpeg -lpng -llzma -lz -lX11

deskew_linear: deskew_linear.cpp
		$(CXX) -Dcimg_use_cpp11 -Dcimg_use_openmp -Dcimg_use_tiff -Dcimg_use_zlib \
		-std=c++11 -fopenmp -pthread -Ofast \
		-Wall -Wextra -Wfatal-errors -Werror=unknown-pragmas -Werror=unused-label \
		-I ./ deskew_linear.cpp -o deskew_linear \
		-lm -ltiff -ljpeg -lpng -llzma -lz -lX11
