#include "CImg.h"
using namespace cimg_library;


CImg<> fourier_deskew(const CImg<> &raw_stack, const float pitch_xy, const float obj_angle, const float stage_step) {
	const float slice_shift = stage_step * std::sin(obj_angle * M_PI / 180) / pitch_xy;
	const int new_height = raw_stack.height() + ceil(fabs(slice_shift) * raw_stack.depth()) - 1;

	printf("Deskewed height: %d\n", new_height);
	printf("Slicewise shift: %4f\n", slice_shift);

	CImgList<> deskewed = raw_stack.get_resize(-100, new_height, -100, -100, 0, 0).get_FFT('y');
	int start_time = cimg::time();

	cimg_forXYZ(deskewed[0], x, y, z) {
		float k = y <= floor(new_height / 2) ? y : -new_height + y;
		k *= -2 * M_PI / new_height;
		float anti_z = raw_stack.depth() - 1 - z;
		float a = deskewed[0](x, y, z);
		float b = deskewed[1](x, y, z);
		deskewed[0](x, y, z) = a * std::cos(k * slice_shift * anti_z) - b * std::sin(k * slice_shift * anti_z);
		deskewed[1](x, y, z) = a * std::sin(k * slice_shift * anti_z) + b * std::cos(k * slice_shift * anti_z);
	}
	deskewed.FFT('y', true);

	int calc_time = cimg::time() - start_time;
	printf("Deskewing time:  %d ms\n", calc_time);
	return deskewed[0];
}


int main(int argc, char * argv[]) {
	cimg_help("Fourier deskewing of stage scan light sheet data");
	const char * file_img = cimg_option("-i", (char*) 0, "input image file");
	const char * file_out = cimg_option("-o", (char*) 0, "output image file");
	const float pitch_xy = cimg_option("-xy", 104, "xy pixel pitch");
	const float obj_angle = cimg_option("-a", 58.2, "objective angle");
	const float stage_step = cimg_option("-s", 500, "stage step");
	const bool display = cimg_option("-d", false, "display deskewed");
	if (!file_img || !file_out) {return 1;}

	int start_time = cimg::time();
	CImg<> img(file_img);
	int in_time = cimg::time() - start_time;
	printf("\n");
	printf("Load time:       %d ms\n", in_time);

	printf("Raw dimensions:  %d x %d x %d\n", img.width(), img.height(), img.depth());
	img = fourier_deskew(img, pitch_xy, obj_angle, stage_step);

	const float new_z = stage_step * std::cos(obj_angle * M_PI / 180);
	const float voxel_size[3] = {pitch_xy, pitch_xy, new_z};
	char description [128];
	sprintf(description, "ImageJ=0.00\nspacing=%f\nunit=nm", new_z);

	start_time = cimg::time();
	img.save_tiff(file_out, 0, voxel_size, description, true);
	int out_time = cimg::time() - start_time;
	printf("Save time:       %d ms\n", out_time);
	printf("\n");

	if (display) {
		img.display();
		printf("\n");
	}

	return 0;
}
