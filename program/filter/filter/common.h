#pragma once

#include "itkImageIO.h"

#include <string>
#include <iostream>

using InputType	= short;
using OutputType = double;
using LabelType	= unsigned char;

template<unsigned int Dim>
class filterInput
{
public:
	filterInput() {};
	filterInput(const std::string& imagename) {
		metadata.Read(input, root + "/org/" + imagename);
		metadata.Read(mask, root + "/mask/" + imagename);
	}
	~filterInput() {};

	ImageIO<Dim> metadata;
	std::vector<InputType> input;
	std::vector<LabelType> mask;
};
