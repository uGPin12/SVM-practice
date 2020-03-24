#pragma once

// Applies a median filter to a 2D image.
// A median filter is used to smooth an image 
// without being biased or shot noise.
//
// median(imgO, imgI, mask, siz, kernel) computes an image 
// where a given pixel is the median value of the pixel 
// in a 8 neighborhood about the corresponding input pixel.
// The output imgO is a grayscale image whose size is siz,
// same as the input imgI.
// The argument kernel_size is the side length of neighborhood 
// and the larger kernel_size, the stronger a smoothing effect.
//
// This function supports mask processing and the outside region 
// of mask has the same pixel value as input pixel.

#include <algorithm>
#include <iostream>
#include <vector>

#include "common.h"

/*
	imgO		: Output image
	imgI		: Input image
	mask		: Mask image
	siz			: The size of image
	kernel_size : The side length of kernel
*/
void median(
	std::vector<output_type>& imgO,
	const std::vector<input_type>& imgI, const std::vector<mask_type>& mask,
	const std::vector<unsigned int>& siz, const unsigned int kernel_size
)
{
	// Check argument.
	if (kernel_size % 2 == 0) {
		std::cerr
			<< "Kernel size must be odd number.\n"
			<< "That is because the center of kernel is unspecified if the kernel size is even.\n"
			<< "Hit any key to exit."
			<< std::endl;
		system("pause");
		std::exit(EXIT_FAILURE);
	}

	if (kernel_size < 3) {
		std::cerr
			<< "Kernel size is too small. Kernel size must be 3 or more.\n"
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
	int half_kernel_size = std::floor(kernel_size / 2);
	std::vector<output_type> kernel;

	// Starts raster scan 
	for (unsigned int y = 0; y < ye; y++) {
		for (unsigned int x = 0; x < xe; x++) {

			unsigned int s = y * xe + x;
			kernel.erase(kernel.begin(), kernel.end());
			if (mask[s] != 0) {
				for (int j = -half_kernel_size; j <= half_kernel_size; j++) {
					for (int i = -half_kernel_size; i <= half_kernel_size; i++) {
						
						// Interpolates pixel value by nearest neighbor if the focus pixel is out of range.
						unsigned int ss = std::max(static_cast<unsigned int>(0), std::min(y + j, ye - 1))* xe
							+ std::max(static_cast<unsigned int>(0), std::min(x + i, xe - 1));
						kernel.push_back(imgI[static_cast<std::size_t>(ss)]);

					}
				}

				std::sort(kernel.begin(), kernel.end());
				imgO[s] = kernel[std::floor(kernel.size() / 2)];
			}
			else
			{
				imgO[s] = static_cast<output_type>(imgI[s]);
			}

		}
	}
	// Ends raster scan
}
