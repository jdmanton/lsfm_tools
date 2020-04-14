deskew_mip: deskew_mip.cpp
		$(CXX) -Dcimg_use_cpp11 -Dcimg_use_openmp -Dcimg_use_tiff -Dcimg_use_zlib \
		-std=c++11 -fopenmp -pthread -Ofast \
		-Wall -Wextra -Wfatal-errors -Werror=unknown-pragmas -Werror=unused-label \
		-I ./ deskew_mip.cpp -o deskew_mip \
		-lm -ltiff -ljpeg -lpng -llzma -lz -lX11
