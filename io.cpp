#include "CImg.h"
#include "tinytiffwriter.h"

using namespace cimg_library;

namespace lsfm {
    void save_tiff(CImg<> &img, const char* filename) {
        // TODO: it's a bit weird that &img can't be const (?)
        // TODO: work out how to write spacing to file using TinyTIFF
        // CImg code looks like:
	    // char description [128];
	    // sprintf(description, "ImageJ=0.00\nspacing=%f\nunit=nm", new_z);
    	// img.save_tiff(file_out, 0, voxel_size, description, true);

        TinyTIFFFile* tiff = TinyTIFFWriter_open(filename, 32, img.width(), img.height());
        if (tiff) {
            for (int slice = 0; slice < img.depth(); slice++) {
                float* data = img.data(0, 0, slice);
                TinyTIFFWriter_writeImage(tiff, data);
            }
            TinyTIFFWriter_close(tiff);
        }
    }
}