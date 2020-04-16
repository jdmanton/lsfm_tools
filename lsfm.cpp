#include "lsfm.h"

int deskew(int argc, char * argv[]) {
  cimg_help("Deskew stage scan light sheet data");
  const char * method = cimg_option("-method", "", "method []");
  const char * file_img = cimg_option("-i", (char*) 0, "input image file");
  const char * file_out = cimg_option("-o", (char*) 0, "output image file");
  const float pitch_xy = cimg_option("-xy", (float)104, "xy pixel pitch");
  const float obj_angle = cimg_option("-a", (float)58.2, "objective angle");
  const float stage_step = cimg_option("-s", (float)500, "stage step");
  const bool display = cimg_option("-d", false, "display MIP");
  if (!file_img || !file_out) {return 1;}

  int start_time = cimg::time();
  CImg<> img(file_img);
  int in_time = cimg::time() - start_time;

  printf("\nLoad time:       %d ms\n", in_time);
  printf("Raw dimensions:  %d x %d x %d\n", img.width(), img.height(), img.depth());

  img = lsfm::deskew(img, pitch_xy, obj_angle, stage_step, method);

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

int deconv(int argc, char * argv[]) {
  return 0;
}

int mip(int argc, char * argv[]) {
  return 0;
}

int process(int argc, char * argv[]) {
  return 0;
}

int main(int argc, char * argv[]) {

  if (argc == 1) {
    printf("\nlsfm [command] [options] \n\n"
	   " Use one of the following command:\n"
	   "  deskew : deskew lightsheet stack\n"
	   "  deconv : deconvolution of a deskewed stack\n"
	   "  mip    : deskew and maximum intensity projection\n\n"
	   " If no command is provided all steps will be performed\n\n");
    return 0;
  }

  if (!strcmp(argv[1], "deskew")) {
    return deskew(argc, argv);
  } else if (!strcmp(argv[1], "deconv")) {
    return deconv(argc, argv);
  } else if (!strcmp(argv[1], "mip")) {
    return mip(argc, argv);
  } else {
    return process(argc, argv);
  }

  return 0;
}
