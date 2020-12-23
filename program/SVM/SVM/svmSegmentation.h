#pragma once

#include "svm.h"
#include "dataIO.h"

#include <fstream>

template <unsigned int Dim, class PixelType>
class svmSegmentation
{
public:
	svm_model* model;
	svm_parameter param;

	svmSegmentation() {};
	svmSegmentation(const std::string& paramFilename, const std::string& probFilename)
	{
		paramRefer(paramFilename);
	};
	~svmSegmentation()
	{
		svm_destroy_param(&param);
		svm_free_and_destroy_model(&model);
	};

	void train(svm_problem& prob);
	void save(const std::string& outputModelname);

private:
	std::string kernel_type;
	int degree;
	double C, gamma, coef0;

	int nObj, nBkg;

	void paramRefer(const std::string& paramFilename)
	{
		kernel_type = read_param_boost<std::string>(paramFilename, "param", "kernel_type");
		if (kernel_type == "liner") param.kernel_type = LINEAR;
		else if (kernel_type == "poly") param.kernel_type = POLY;
		else if (kernel_type == "rbf") param.kernel_type = RBF;
		else if (kernel_type == "sigmoid") param.kernel_type = SIGMOID;
		else {
			std::cerr << "Not defined for the specified kernel type.\n"
				<< "Hit any key to exit..." << std::endl;
			system("pause");
			std::exit(EXIT_FAILURE);
		}
		
		param.C			= read_param_boost<double>(paramFilename, "param", "C");
		param.gamma		= read_param_boost<double>(paramFilename, "param", "gamma");
		param.degree	= read_param_boost<int>(paramFilename, "param", "degree");
		param.coef0		= read_param_boost<double>(paramFilename, "param", "coef0");
		
		param.svm_type = C_SVC;
		param.eps = 1e-3;
		param.cache_size = 100;
		param.probability = 0;
		param.nu = 0.5;
		param.p = 0.1;
		param.nr_weight = 0;
		param.weight_label = nullptr;
		param.weight = nullptr;
		param.shrinking = 0;

		nObj = read_param_boost<int>(paramFilename, "param", "nObj");
		nBkg = read_param_boost<int>(paramFilename, "param", "nBkg");
	}
};

template <unsigned int Dim, class PixelType>
void svmSegmentation<Dim, PixelType>::train(svm_problem& prob)
{
	std::cout << "------ Start training ------" << std::endl;
	model = svm_train(&prob, &param);
	std::cout << "------ Complete training ------" << std::endl;
}

template <unsigned int Dim, class PixelType>
void svmSegmentation<Dim, PixelType>::save(const std::string& outputModelname)
{
	std::cout << "------Save: " << outputModelname << " ------" << std::endl;
	svm_save_model(outputModelname.c_str(), model);
}
