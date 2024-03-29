#include "CImg.h"
using namespace cimg_library;

namespace lsfm {
	CImg<> deskew(const CImg<> &raw_stack, const float pitch_xy, const float obj_angle, const float stage_step, const char* method);
	CImg<> proj(const CImg<> & raw_stack, const char* method);
	CImg<> deconv(const CImg<> &raw_stack, const CImgList<> &psf, const char* method, const int num_iters);
	CImgList<> psf2otf(const CImg<>& psf, const int width, const int height, const int depth);
	CImg<> load_tiff(const char* filename);
	void save_tiff(CImg<> &img, const char* filename, float pitch_xy, float spacing_z);
}
