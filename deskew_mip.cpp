#include "CImg.h"
using namespace cimg_library;


CImg<> deskew_mip(const CImg<> &raw_stack, const float pitch_xy, const float obj_angle, const float stage_step) {
	const float slice_shift = stage_step * std::sin(obj_angle * M_PI / 180) / pitch_xy;
	const int offset = ceil(fabs(slice_shift) * raw_stack.depth()) - 1;
	const int new_height = raw_stack.height() + offset;

	printf("Deskewed height: %d\n", new_height);
	printf("Slicewise shift: %4f\n", slice_shift);

	CImg<> mip(raw_stack.width(), new_height, 1, 1, 0);
	int start_time = cimg::time();

	// No interpolation
	// cimg_forXYZ(raw_stack, x, y, z) {
	// 	int mip_y = offset + round(y - z * slice_shift);
	// 	mip(x, mip_y) = raw_stack(x, y, z) > mip(x, mip_y) ? raw_stack(x, y, z) : mip(x, mip_y);
	// }

	// Linear interpolation
	float y0 = slice_shift;
	float y1 = floor(slice_shift);
	float y2 = ceil(slice_shift);
	cimg_forXYZ(raw_stack, x, y, z) {
		float f1 = raw_stack(x, y, z);
		float f2 = y < raw_stack.height() - 1 ? raw_stack(x, y + 1, z) : 0;
		float interp = f1 * (y2 - y0) / (y2 - y1) + f2 * (y0 - y1) / (y2 - y1);

		int mip_y = offset + round(y - z * slice_shift);
		mip(x, mip_y) = interp > mip(x, mip_y) ? interp : mip(x, mip_y);
	}

	int calc_time = cimg::time() - start_time;
	printf("MIP time:        %d ms\n", calc_time);
	return mip;
}


int main(int argc, char * argv[]) {
	cimg_help("Pseudo-deskew MIP of stage scan light sheet data");
	const char * file_img = cimg_option("-i", (char*) 0, "input image file");
	const char * file_out = cimg_option("-o", (char*) 0, "output image file");
	const float pitch_xy = cimg_option("-xy", 104, "xy pixel pitch");
	const float obj_angle = cimg_option("-a", 58.2, "objective angle");
	const float stage_step = cimg_option("-s", 500, "stage step");
	const bool display = cimg_option("-d", false, "display MIP");
	if (!file_img || !file_out) {return 1;}
	CImg<> img(file_img);

	printf("\n");
	printf("Raw dimensions:  %d x %d x %d\n", img.width(), img.height(), img.depth());
	img = deskew_mip(img, pitch_xy, obj_angle, stage_step);
	printf("\n");

	const float voxel_size[3] = {pitch_xy, pitch_xy, 1};
	const char * description = "ImageJ=0.00\nspacing=1\nunit=nm";
	img.save_tiff(file_out, 0, voxel_size, description, true);
	if (display) {
		img.display();
		printf("\n");
	}

	return 0;
}
