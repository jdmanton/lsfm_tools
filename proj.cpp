#include "CImg.h"

using namespace cimg_library;

namespace lsfm {
	/*
		Calculate the maximum of all values along z
	*/
	CImg<> mip(const CImg<> &raw_stack, CImg<> &proj) {
		cimg_forXYZ(raw_stack, x, y, z) {
			proj(x, y) = raw_stack(x, y, z) > proj(x, y) ? raw_stack(x, y, z) : proj(x, y);
		}
		return proj;
	}


	/*
		Calculate the mean of all values along z
	*/
	CImg<> mean(const CImg<> &raw_stack, CImg<> &proj) {
		cimg_forXYZ(raw_stack, x, y, z) {
			proj(x, y) += raw_stack(x, y, z) / raw_stack.depth();
		}
		return proj;
	}


	/*
		Smooth the stack with a Gaussian, find the Laplacian and then max project
	*/
	CImg<> laplacian_mip(const CImg<> &raw_stack, CImg<> &proj) {
		CImg<> gaussian = raw_stack.get_blur(2);
		CImg<> laplacian(raw_stack.width(), raw_stack.height(), raw_stack.depth(), 1, 0);
		
		CImg_3x3(I, float); 
		cimg_forZ(gaussian, z) {
			cimg_for3x3(gaussian, x, y, z, 0, I, float) {
				laplacian(x, y, z) = Icp + Ipc + Inc + Icn - 4 * Icc;
			}
		}
		return mip(laplacian, proj);
	}


	CImg<> proj(const CImg<> &raw_stack, const char* method) {
		CImg<> to_return;
		CImg<> proj(raw_stack.width(), raw_stack.height(), 1, 1, 0);

		printf("\nProjecting data using ");
		int start_time = cimg::time();

		if (!strcmp(method, "laplacian")) {
			printf("Laplacian method...\n");
			to_return = laplacian_mip(raw_stack, proj);
		} else if (!strcmp(method, "mean")) {
			printf("mean intensity...\n");
			to_return = mean(raw_stack, proj);
		} else {
			printf("maximum intensity...\n");
			to_return = mip(raw_stack, proj);
		}
		
		int calc_time = cimg::time() - start_time;
		printf("Projection time: %d ms\n", calc_time);

		return to_return;
	}
}
