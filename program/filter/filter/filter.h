#pragma once

#include <vector>
#include <iomanip>
#include "common.h"
#include "itkImageIO.h"

void median(
	std::vector<output_type>& imgO,
	const std::vector<input_type>& imgI, const std::vector<mask_type>& mask,
	const std::vector<unsigned int>& siz, const unsigned int kernel_size
);

void dilation(
	std::vector<output_type>& imgO,
	const std::vector<input_type>& imgI, const std::vector<mask_type>& mask,
	const std::vector<unsigned int>& siz, const unsigned int kernel_size
);

void erosion(
	std::vector<output_type>& imgO,
	const std::vector<input_type>& imgI, const std::vector<mask_type>& mask,
	const std::vector<unsigned int>& siz, const unsigned int kernel_size
);

void opening(
	std::vector<output_type>& imgO,
	const std::vector<input_type>& imgI, const std::vector<mask_type>& mask,
	const std::vector<unsigned int>& siz, const unsigned int kernel_size
);

void closing(
	std::vector<output_type>& imgO,
	const std::vector<input_type>& imgI, const std::vector<mask_type>& mask,
	const std::vector<unsigned int>& siz, const unsigned int kernel_size
);

void LoG(
	std::vector<output_type>& imgO,
	const std::vector<input_type>& imgI, const std::vector<mask_type>& mask,
	const std::vector<unsigned int>& siz, const double sigma
);

void sharpening_4N(
	std::vector<output_type>& imgO,
	std::vector<input_type>& imgI, std::vector<mask_type>& mask,
	const std::vector<unsigned int> siz, const int k
);

void sharpening_8N(
	std::vector<output_type>& imgO,
	std::vector<input_type>& imgI, std::vector<mask_type>& mask,
	const std::vector<unsigned int> siz, const int k
);

void sobel(
	std::vector<output_type>& imgO,
	const std::vector<input_type>& imgI, const std::vector<mask_type>& mask,
	const std::vector<unsigned int>& siz, const unsigned int mode
);

void convergence(
	std::vector<output_type>& imgO,
	const std::vector<input_type>& imgI, const std::vector<mask_type>& mask,
	const std::vector<unsigned int>& siz,
	const int N, const int r_min, const int r_max, const int d
);

void bilateral(
	std::vector<output_type>& imgO,
	std::vector<input_type>& imgI, std::vector<mask_type>& mask,
	const std::vector<unsigned int>& siz, const double sigma_s, double sigma_c
);