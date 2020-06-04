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

        int start_time = cimg::time();
        TinyTIFFFile* tiff = TinyTIFFWriter_open(filename, 32, img.width(), img.height());
        if (tiff) {
            for (int slice = 0; slice < img.depth(); slice++) {
                float* data = img.data(0, 0, slice);
                TinyTIFFWriter_writeImage(tiff, data);
            }
            TinyTIFFWriter_close(tiff);
        }
        int out_time = cimg::time() - start_time;
        printf("Save time:       %d ms\n", out_time);
        printf("\n");
    }

    CImg<> load_tiff(const char* filename) {
        int start_time = cimg::time();
        CImg<> img(filename);
        int in_time = cimg::time() - start_time;
        printf("\nLoad time:       %d ms\n", in_time);
        printf("Raw dimensions:  %d x %d x %d\n", img.width(), img.height(), img.depth());

        return img;
    }
}