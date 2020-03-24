#pragma once

#include <algorithm>
#include <iostream>
#include <vector>

#include "common.h"

void identity(
	std::vector<output_type>& imgO,
	const std::vector<input_type>& imgI, const std::vector<unsigned int>& siz,
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

	unsigned int xe = siz[0];
	unsigned int ye = siz[1];

	for (unsigned int y = 0; y < ye; y++) {
		for (unsigned int x = 0; x < xe; x++) {

			unsigned int s = y * xe + x;
			
			imgO[s] = static_cast<output_type>(imgI[s]);
		}
	}


}