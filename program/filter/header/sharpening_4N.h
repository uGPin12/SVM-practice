#pragma once

#include <algorithm>
#include <iostream>
#include <vector>

#include "common.h"

void sharpening_4N(
	std::vector<output_type>& imgO,
	std::vector<input_type>& imgI, std::vector<mask_type>& mask,
	const std::vector<unsigned int> siz, const int k
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

	std::vector<std::vector<int>> kernel = {
		{	 0,			-k,	 0},
		{	-k,	 1 + 4 * k,	-k},
		{	 0,			-k,	 0}
	};

	unsigned int xe = siz[0];
	unsigned int ye = siz[1];

	for (unsigned int y = 0; y < ye; y++) {
		for (unsigned int x = 0; x < xe; x++) {

			unsigned int s = y * xe + x;
			if (mask[s] != 0) {
				output_type pixel_value = 0;
				for (int j = -1; j <= 1; j++) {
					for (int i = -1; i <= 1; i++) {

						unsigned int ss = std::max(static_cast<unsigned int>(0), std::min(y + j, ye - 1))* xe
							+ std::max(static_cast<unsigned int>(0), std::min(x + i, xe - 1));
						pixel_value += static_cast<output_type>(kernel[j + 1][i + 1]) * static_cast<output_type>(imgI[ss]);

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