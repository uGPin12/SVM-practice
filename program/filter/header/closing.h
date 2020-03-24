#pragma once

#include <algorithm>
#include <iostream>
#include <vector>

#include "common.h"

void closing(
	std::vector<output_type>& imgO,
	const std::vector<input_type>& imgI, const std::vector<mask_type>& mask,
	const std::vector<unsigned int>& siz, const unsigned int kernel_size
)
{

	if (kernel_size < 3) {
		std::cerr << "Kernel size must be 3 or more.\n"
			<< "Hit any key to exit."
			<< std::endl;
		system("pause");
		std::exit(EXIT_FAILURE);
	}

	if (kernel_size % 2 == 0) {
		std::cerr << "Kernel size must be odd number.\n"
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
	std::vector<output_type> buf(imgO.size());
	int half_kernel_size = std::floor(kernel_size / 2);

	unsigned int x, y;
	int i, j;
	// dilation
	for (y = 0; y < ye; y++) {
		for (x = 0; x < xe; x++) {

			unsigned int s = y * xe + x;
			if (mask[s] != 0) {
				output_type max_value = static_cast<output_type>(INT_MIN);
				for (j = -half_kernel_size; j <= half_kernel_size; j++) {
					for (i = -half_kernel_size; i <= half_kernel_size; i++) {
						unsigned int ss = std::max(static_cast<unsigned int>(0), std::min(y + j, ye - 1))* xe
							+ std::max(static_cast<unsigned int>(0), std::min(x + i, xe - 1));

						if (static_cast<output_type>(imgI[static_cast<std::size_t>(ss)]) > max_value) {
							max_value = static_cast<output_type>(imgI[static_cast<std::size_t>(ss)]);
						}
					}
				}
				buf[s] = max_value;
			}
			else
			{
				buf[s] = static_cast<output_type>(imgI[s]);
			}

		}
	}

	// erosion
	for (y = 0; y < ye; y++) {
		for (x = 0; x < xe; x++) {

			std::size_t s = static_cast<std::size_t>(y * xe + x);
			if (mask[s] != 0) {
				output_type min_value = static_cast<output_type>(INT_MAX);
				for (j = -half_kernel_size; j <= half_kernel_size; j++) {
					for (i = -half_kernel_size; i <= half_kernel_size; i++) {
						unsigned int ss = std::max(static_cast<unsigned int>(0), std::min(y + j, ye - 1))* xe
							+ std::max(static_cast<unsigned int>(0), std::min(x + i, xe - 1));

						if (buf[static_cast<std::size_t>(ss)] < min_value) {
							min_value = buf[static_cast<std::size_t>(ss)];
						}
					}
				}
				imgO[s] = min_value;
			}
			else
			{
				imgO[s] = buf[s];
			}

		}
	}
}