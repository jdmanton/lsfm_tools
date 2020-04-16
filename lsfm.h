#include "CImg.h"
using namespace cimg_library;

namespace lsfm {
  CImg<> deskew(const CImg<> &raw_stack,
		const float pitch_xy,
		const float obj_angle,
		const float stage_step,
		const char * method);

  CImg<> deskew_mip(const CImg<> &raw_stack,
		    const float pitch_xy,
		    const float obj_angle,
		    const float stage_step,
		    const char * method);

  CImg<> deconvolve(const CImg<> &raw_stack);

}
