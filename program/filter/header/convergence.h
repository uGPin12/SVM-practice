#pragma once

#include <algorithm>
#include <iostream>
#include <vector>

#include "common.h"
#include "sobel.h"

void convergence(
	std::vector<output_type>& imgO,
	const std::vector<input_type>& imgI, const std::vector<mask_type>& mask,
	const std::vector<unsigned int>& siz,
	const int N, const int r_min, const int r_max, const int d
)
{
	if (siz.size() != 2)
	{
		std::cerr << "This function does not support except for 2D image.\n"
			<< "Hit any key to exit."
			<< std::endl;
		system("pause");
		std::exit(EXIT_FAILURE);
	}

	std::vector<output_type> x_grad(imgI.size()), y_grad(imgI.size());
	std::vector<mask_type> buf_mask(imgI.size(), 1);
	sobel(x_grad, imgI, buf_mask, siz, 0);
	sobel(y_grad, imgI, buf_mask, siz, 1);

	unsigned int xe = siz[0];
	unsigned int ye = siz[1];
	double theta = 2.0 * PI / N;

	for (unsigned int y = 0; y < ye; y++) {
		for (unsigned int x = 0; x < xe; x++) {
			output_type max_v = static_cast<output_type>(INT_MIN);
			unsigned int s = y * xe + x;
			if (mask[s] != 0) {

				for (int r1 = r_min; r1 < r_max - d + 2; r1++) {
					output_type pixel_value = 0.0;
					for (int r2 = r1; r2 < r1 + d; r2++) {

						for (int a = 0; a < N; a++) {

							output_type x_l = static_cast<output_type>(r2 * std::cos(theta * a));
							output_type y_l = static_cast<output_type>(r2 * std::sin(theta * a));
							int X = x + static_cast<int>(std::floor(static_cast<double>(r2 * std::cos(-theta * a)) + 0.5));
							int Y = y + static_cast<int>(std::floor(static_cast<double>(r2 * std::sin(-theta * a)) + 0.5));
							unsigned int ss = std::max(-std::min(Y, 2 * (static_cast<int>(ye) - 1) - Y), std::min(Y, 2 * (static_cast<int>(ye) - 1) - Y))* xe
								+ std::max(-std::min(X, 2 * (static_cast<int>(xe) - 1) - X), std::min(X, 2 * (static_cast<int>(xe) - 1) - X));
							if (mask[ss] != 0) {
								if (x_grad[ss] != 0 || y_grad[ss] != 0) {
									output_type f1 = x_l * x_grad[ss] + y_l * y_grad[ss];
									output_type f2 = std::sqrt(x_l * x_l + y_l * y_l) * std::sqrt(x_grad[ss] * x_grad[ss] + y_grad[ss] * y_grad[ss]);
									pixel_value += f1 / f2 / static_cast<double>(N * d);
								}
							}

						}

					}
					if (pixel_value > max_v) max_v = pixel_value;
				}
				imgO[s] = static_cast<output_type>(max_v * 1000.0);
			}
			else {
				imgO[s] = static_cast<output_type>(imgI[s]);
			}
		}
	}
}