#include "CImg.h"

using namespace cimg_library;

namespace lsfm {
	CImg<> barl(const CImgList<> &H, const CImg<> &f, const int niter, const float bg) {
		const float eps = 1e-2;
		CImg<> u(f.width(), f.height(), f.depth(), 1, f.mean() - bg);
		CImgList<> tmp(2, u.width(), u.height(), u.depth(), 1, 0);
		CImg<> u_prev(u.width(), u.height(), u.depth(), 1, f.mean() - bg);
		CImg<> u_new(u.width(), u.height(), u.depth(), 1, 0);
		float a = 0;
		CImg<> g_prev(u.width(), u.height(), u.depth(), 1, 0);
		CImg<> g_curr(u.width(), u.height(), u.depth(), 1, 0);

		for (int iter = 0; iter < niter; iter++) {
			int start_time = cimg::time();

			a = g_prev.sum() == 0 ? 0 : (g_curr.get_mul(g_prev)).sum() / (g_prev.get_mul(g_prev)).sum();
			a = a > 1 ? 1 : a;
			a = a < 0 ? 0 : a;

			u_new = u + a * (u - u_prev);
			u_prev = u;
			u = u_new;
			//compute H*u = ifft(H * fft(u))
			tmp = u.get_FFT();
			cimg_foroff(H[0],i) {
				const float a = tmp[0](i), b = tmp[1](i), c = H[0](i), d = H[1](i);
				tmp[0](i) = a*c - b*d;
				tmp[1](i) = a*d + b*c;
			}
			tmp.FFT(true);
			// compute the ratio f / Hu
			cimg_foroff(tmp[0],i) {
				if (tmp[0](i) > 0)
					tmp[0](i) = std::max(eps, f(i) - bg) / tmp[0](i);
				tmp[1](i) = 0;
			}
			// compute H' (f/Hu)
			tmp = tmp.FFT();
			cimg_foroff(H[0],i) {
				const float a = tmp[0](i), b = tmp[1](i), c = H[0](i), d = -H[1](i);
				tmp[0](i) = a*c - b*d;
				tmp[1](i) = a*d + b*c;
			}
			tmp.FFT(true);
			// update u = u * H' (f/Hu)
			u.mul(tmp[0]);
			u.max(0.0f);
			g_prev = g_curr;
			g_curr = u - u_prev;

			int iter_time = cimg::time() - start_time;
			printf("Iteration %03d: alpha = %1.3f, t = %3d ms\n", iter + 1, a, iter_time);
		}
		return u;
	}


	CImg<> richardson_lucy(const CImgList<> &H, const CImg<> &f, const int niter, const float bg) {
		const float eps = 1e-2;
		CImg<> u((f-bg).max(eps).blur(4.0f));
		CImgList<> tmp(2, f.width(), f.height(), f.depth(), 1, 0);
		for (int iter = 0; iter < niter; iter++) {
			int start_time = cimg::time();
			//compute H*u = ifft(H * fft(u))
			tmp = u.get_FFT();
			cimg_foroff(H[0],i) {
				const float a = tmp[0](i), b = tmp[1](i), c = H[0](i), d = H[1](i);
				tmp[0](i) = a*c - b*d;
				tmp[1](i) = a*d + b*c;
			}
			tmp.FFT(true);
			// compute the ration f / Hu
			cimg_foroff(tmp[0],i) {
				if (tmp[0](i) > 0)
					tmp[0](i) = std::max(eps, f(i) - bg) / tmp[0](i);
				tmp[1](i) = 0;
			}
			// compute H' (f/Hu)
			tmp = tmp.FFT();
			cimg_foroff(H[0],i) {
				const float a = tmp[0](i), b = tmp[1](i), c = H[0](i), d = -H[1](i);
				tmp[0](i) = a*c - b*d;
				tmp[1](i) = a*d + b*c;
			}
			tmp.FFT(true);
			// update u = u * H' (f/Hu)
			u.mul(tmp[0]);
			int iter_time = cimg::time() - start_time;
			printf("Iteration %03d: t = %3d ms\n", iter + 1, iter_time);
		}
		return u;
	}


	CImg<> deconv(const CImg<> &raw_stack, const CImgList<> &otf, const char* method, const int num_iters) {
		CImg<> deconvolved;
		int start_time = cimg::time();
		printf("\nDeconvolving data using ");
		if (!strcmp(method, "fourier")) {
			printf("Fourier method...\n");
		} else if (!strcmp(method, "barl")) {
			printf("Biggs-Andrews accelerated Richardson-Lucy method...\n");
			deconvolved = barl(otf, raw_stack, num_iters, 100);
		} else {
			printf("Richardson-Lucy method...\n");
			deconvolved = richardson_lucy(otf, raw_stack, num_iters, 100);
		}
		int calc_time = cimg::time() - start_time;
		printf("Deconvolution time:     %d ms\n", calc_time);
		
		return deconvolved;
	}


	CImgList<> psf2otf(const CImg<>& psf, const int width, const int height, const int depth) {
		int x_max_ind = 0, y_max_ind = 0, z_max_ind = 0;
		double v_max = 0, sum = 0;
		cimg_forXYZ(psf, x, y, z) {
			const double v = psf(x, y, z);
			sum += v;
			if (v > v_max) {
				v_max = v;
				x_max_ind = x;
				y_max_ind = y;
				z_max_ind = z;
			}
		}
		CImg<> h(width, height, depth, 1, 0);
		const int
			x0 = cimg::round((float) width / 2.0f - (float) x_max_ind),
			y0 = cimg::round((float) width / 2.0f - (float) y_max_ind),
			z0 = depth == 1 ? 0 : round((float) depth / 2.0f - (float) z_max_ind);
		h.draw_image(x0, y0, z0, psf);
		h = h / (float) sum;
		h.shift(width / 2, height / 2, depth / 2, 0, 2);
		return h.get_FFT();
	}
}
