#pragma once

#include <algorithm>
#include <iostream>
#include <vector>

#include "common.h"

void sobel(
	std::vector<output_type>& imgO,
	const std::vector<input_type>& imgI, const std::vector<mask_type>& mask,
	const std::vector<unsigned int>& siz, const unsigned int mode
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

	if (mode > 2) {
		std::cerr << "Invalid input : 'mode' must be 0 or 1 or 2.\n"
			<< "Hit any key to exit."
			<< std::endl;
		system("pause");
		std::exit(EXIT_FAILURE);
	}

	std::vector<std::vector<output_type>> x_weight = {
		{-1,0,1},
		{-2,0,2},
		{-1,0,1}
	};
	std::vector<std::vector<output_type>> y_weight = {
		{ 1,  2,  1},
		{ 0,  0,  0},
		{-1, -2, -1}
	};

	unsigned int xe = siz[0];
	unsigned int ye = siz[1];

	for (unsigned int y = 0; y < ye; y++) {
		for (unsigned int x = 0; x < xe; x++) {

			unsigned int s = y * xe + x;
			if (mask[s] != 0) {
				output_type x_pixel_value = 0.0, y_pixel_value = 0.0;
				for (int j = -1; j <= 1; j++) {
					for (int i = -1; i <= 1; i++) {

						unsigned int ss = std::max(static_cast<unsigned int>(0), std::min(y + j, ye - 1))* xe
							+ std::max(static_cast<unsigned int>(0), std::min(x + i, xe - 1));
						x_pixel_value += x_weight[j + 1][i + 1] * static_cast<output_type>(imgI[ss]);
						y_pixel_value += y_weight[j + 1][i + 1] * static_cast<output_type>(imgI[ss]);
					
					}

					if (mode == 0)		imgO[s] = x_pixel_value;
					else if (mode == 1) imgO[s] = y_pixel_value;
					else if (mode == 2) imgO[s] = std::sqrt(x_pixel_value * x_pixel_value + y_pixel_value * y_pixel_value);
				}
			}
			else {
				imgO[s] = static_cast<output_type>(imgI[s]);
			}
		}
	}


}