#include "CImg.h"

using namespace cimg_library;

namespace lsfm {
	/*
		Use a 1D FFT and a ramped phase ramp to shift all slices together
	*/
	CImg<> deskew_fft(const CImg<> &raw_stack, const int new_height, const int offset, const float slice_shift) {
		CImgList<> deskewed = raw_stack.get_resize(-100, new_height, -100, -100, 0, 0).get_FFT('y');
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
		return deskewed[0];
	}


	/*
		Use a 1D nearest-neighbour interpolation to very quickly (and very poorly) calculate shifted values
	*/
	CImg<> deskew_nn(const CImg<> &raw_stack, const int new_height, const int offset, const float slice_shift) {
		CImg<> deskewed(raw_stack.width(), new_height, raw_stack.depth(), 1, 0);
		cimg_forXYZ(raw_stack, x, y, z) {
			int deskewed_y = offset + round(y - z * slice_shift);
			deskewed(x, deskewed_y) = raw_stack(x, y, z) > deskewed(x, deskewed_y) ? raw_stack(x, y, z) : deskewed(x, deskewed_y);
		}
		return deskewed;
	}


	/*
		Use a 1D linear interpolation to quickly calculate shifted values
	*/
	CImg<> deskew_linear(const CImg<> &raw_stack, const int new_height, const int offset, const float slice_shift) {
		CImg<> deskewed(raw_stack.width(), new_height, raw_stack.depth(), 1, 0);
		float y0 = slice_shift;
		float y1 = floor(slice_shift);
		float y2 = ceil(slice_shift);
		cimg_forXYZ(raw_stack, x, y, z) {
			float f1 = raw_stack(x, y, z);
			float f2 = y < raw_stack.height() - 1 ? raw_stack(x, y + 1, z) : 0;
			float interp = f1 * (y2 - y0) / (y2 - y1) + f2 * (y0 - y1) / (y2 - y1);
			int deskewed_y = offset + round(y - z * slice_shift);
			deskewed(x, deskewed_y, z) = interp;
		}
		return deskewed;
	}


	CImg<> deskew(const CImg<> &raw_stack, const float pitch_xy, const float obj_angle, const float stage_step, const char * method) {
		const float slice_shift = stage_step * std::sin(obj_angle * M_PI / 180) / pitch_xy;
		const int offset = ceil(fabs(slice_shift) * raw_stack.depth()) - 1;
		const int new_height = raw_stack.height() + offset;

		printf("Deskewed height: %d\n", new_height);
		printf("Slicewise shift: %4f\n", slice_shift);

		CImg<> deskewed;
		int start_time = cimg::time();
		printf("\nDeskewing data using ");
		if (!strcmp(method, "fft")) {
			printf("Fourier method...\n");
			deskewed = deskew_fft(raw_stack, new_height, offset, slice_shift);
		} else if (!strcmp(method, "nn")) {
			printf("nearest-neighbour method...\n");
			deskewed = deskew_nn(raw_stack, new_height, offset, slice_shift);
		} else {
			printf("linear method...\n");
			deskewed = deskew_linear(raw_stack, new_height, offset, slice_shift);
		}
		int calc_time = cimg::time() - start_time;
		printf("Deskew time:     %d ms\n", calc_time);
		
		return deskewed;
	}
}
