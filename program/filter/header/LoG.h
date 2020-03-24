#pragma once

#include <algorithm>
#include <iostream>
#include <vector>

#include "common.h"

void LoG(
	std::vector<output_type>& imgO,
	const std::vector<input_type>& imgI, const std::vector<mask_type>& mask,
	const std::vector<unsigned int>& siz, const double sigma
)
{

	if (sigma <= 0) {
		std::cerr << "Sigma must be positive value.\n"
			<< "Hit any key to exit."
			<< std::endl;
		system("pause");
		std::exit(EXIT_FAILURE);
	}

	if (siz.size() != 2)
	{
		std::cerr << "This function does not support except for 2D image.\n"
			<< "Hit any key to exit."
			<< std::endl;
		system("pause");
		std::exit(EXIT_FAILURE);
	}

	unsigned int xe = siz[0];
	unsigned int ye = siz[1];

	int half_kernel_size = std::floor(3 * sigma);
	if (half_kernel_size < 1) half_kernel_size = 1;
	int side_length_kernel = 2 * half_kernel_size + 1;

	output_type sigma_2_power = static_cast<output_type>(std::pow(sigma, 2));
	output_type sigma_6_power = static_cast<output_type>(std::pow(sigma, 6));
	constexpr output_type PI = 3.14159265358979323846;

	for (unsigned int y = 0; y < ye; y++) {
		for (unsigned int x = 0; x < xe; x++) {

			unsigned int s = y * xe + x;
			if (mask[s] != 0) {
				output_type pixel_value = 0;
				output_type k;
				for (int j = -half_kernel_size; j < half_kernel_size; j++) {
					for (int i = -half_kernel_size; i < half_kernel_size; i++) {
						unsigned int ss = std::max(static_cast<unsigned int>(0), std::min(y + j, ye - 1))* xe
							+ std::max(static_cast<unsigned int>(0), std::min(x + i, xe - 1));
						k = static_cast<output_type>(i * i + j * j);
						pixel_value += (k - 2.0 * sigma_2_power)
							/ (2.0 * PI * sigma_6_power) * std::exp((-1.0) * k / (2.0 * sigma_2_power))
							* static_cast<output_type>(imgI[static_cast<std::size_t>(ss)]);
					}
				}
				imgO[s] = pixel_value;
			}
			else
			{
				imgO[s] = static_cast<output_type>(imgI[s]);
			}
		}
	}

}