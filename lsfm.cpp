#include "lsfm.h"

int deskew(int argc, char* argv[]) {
	cimg_help("\nDeskew stage scan light sheet data");
	const char* method =     cimg_option("-m", "linear", "method [linear, nn, fourier]");
	const char* file_img =   cimg_option("-i", (char*) 0, "input image file");
	const char* file_out =   cimg_option("-o", (char*) 0, "output image file");
	const float pitch_xy =   cimg_option("-xy", (float) 104, "xy pixel pitch");
	const float obj_angle =  cimg_option("-a", (float) 58.2, "objective angle");
	const float stage_step = cimg_option("-s", (float) 500, "stage step");
	const bool display =     cimg_option("-d", false, "display deskewed stack\n");
	if (!file_img || !file_out) {return 1;}

	CImg<> img = lsfm::load_tiff(file_img);
	img = lsfm::deskew(img, pitch_xy, obj_angle, stage_step, method);
	float spacing_z = stage_step * std::cos(obj_angle * M_PI / 180);
	lsfm::save_tiff(img, file_out, pitch_xy, spacing_z);

	if (display) {
		img.display("Deskewed stack", false);
	}
	return 0;
}


int deconv(int argc, char* argv[]) {
	cimg_help("\nDeconvolve stack using given PSF");
	const char* method = cimg_option("-m", "rl", "method [rl, barl]");
	const char* file_img = cimg_option("-i", (char*) 0, "input image file");
	const char* file_psf = cimg_option("-p", (char*) 0, "input PSF file");
	const char* file_out = cimg_option("-o", (char*) 0, "output image file");
	const int num_iters = cimg_option("-n", 10, "number of iterations");
	const float pitch_xy = cimg_option("-xy", (float) 104, "xy pixel pitch");
	const float spacing_z = cimg_option("-z", (float) 250, "z pixel pitch");
	const bool display = cimg_option("-d", false, "display deconvolved stack\n");
	if(!file_img || !file_psf || !file_out) {return 1;}

	printf("Loading raw stack...\n");
	CImg<> img = lsfm::load_tiff(file_img);
	printf("Loading PSF...\n");
	CImg<> psf = lsfm::load_tiff(file_psf);
	printf("Creating OTF...\n");
	CImgList<> otf = lsfm::psf2otf(psf, img.width(), img.height(), img.depth());
	img = lsfm::deconv(img, otf, method, num_iters);
	lsfm::save_tiff(img, file_out, pitch_xy, spacing_z);

	if(display) {
		img.display("Deconvolved stack", false);
	}

	return 0;
}


int proj(int argc, char* argv[]) {
	cimg_help("\nProject stack to 2D image");
	const char* method =   cimg_option("-m", "mip", "method [mip, mean, laplacian]");
	const char* file_img = cimg_option("-i", (char*) 0, "input image file");
	const char* file_out = cimg_option("-o", (char*) 0, "output image file");
	const float pitch_xy = cimg_option("-xy", (float) 104, "xy pixel pitch");
	const bool display =   cimg_option("-d", false, "display projection\n");
	if (!file_img || !file_out) {return 1;}

	CImg<> img = lsfm::load_tiff(file_img);
	img = lsfm::proj(img, method);
	lsfm::save_tiff(img, file_out, pitch_xy, 0);

	if (display) {
		img.display("Stack projection", false);
	}
	return 0;
}


int show(int argc, char* argv[]) {
	cimg_help("\nDisplay stack");
	CImg<> img = lsfm::load_tiff(argv[2]);
	img.display("Stack", false);
	printf("\n");
	return 0;
}


int main(int argc, char* argv[]) {
	if (argc == 1) {
		printf("\nlsfm [command] [options] \n\n"
			   "Use one of the following commands:\n"
			   "  show               : show stack\n"
			   "  deskew             : deskew stack\n"
			   "  deconv             : deconvolve stack\n"
			   "  proj               : project stack\n"
			   "  deskew+deconv      : deskew and deconvolve stack\n"
			   "  deskew+proj        : deskew and project stack\n"
			   "  deconv+proj        : deconvolve and project stack\n"
			   "  deskew+deconv+proj : deskew, deconvolve and project stack\n\n"
			   "Multi-operation commands will save each step as a separate file.\n"
			   "Use -h as an option to learn about each command.\n\n");
		return 0;
	}

	if (!strcmp(argv[1], "-h")) {
		main(1, argv);
	} else if (!strcmp(argv[1], "show")) {
		return show(argc, argv);
	} else if (!strcmp(argv[1], "deskew")) {
		return deskew(argc, argv);
	} else if (!strcmp(argv[1], "deconv")) {
		return deconv(argc, argv);
	} else if (!strcmp(argv[1], "proj")) {
		return proj(argc, argv);
	} else {
		printf("Command %s not implemented.\n", argv[1]);
	}

	return 0;
}
