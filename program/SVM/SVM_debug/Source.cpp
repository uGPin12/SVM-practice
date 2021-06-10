#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <manipulations.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/format.hpp>

#include "svm.h"
#include "util.h"

int main(int argc, char* argv[]) {

	if (argc != 2) {
		std::cerr
			<< "Usage: " << argv[0]
			<< " input_info.ini" << std::endl;
		system("pause");
		return EXIT_FAILURE;
	}
	std::string path = argv[1];

	std::string train_path = read_param_boost<std::string>(path, "path", "train_path");
	std::string label_path = read_param_boost<std::string>(path, "path", "label_path");
	std::string test_path = read_param_boost<std::string>(path, "path", "test_path");
	std::string dir_o = read_param_boost<std::string>(path, "path", "dir_o");
	std::string model_name = read_param_boost <std::string>(path, "param", "model_name");
	mkdir_boost(dir_o);

	constexpr std::size_t n_feat = 1;
	svm_problem prob;
	set_svm_input<n_feat>(prob, train_path, label_path);

	svm_parameter param;

	param.svm_type = C_SVC;
	param.kernel_type = RBF;
	param.C = 10;
	param.gamma = 1;

	param.coef0 = 0;
	param.cache_size = 100;
	param.eps = 1e-3;
	param.shrinking = 1;
	param.probability = 0;

	param.degree = 3;
	param.nu = 0.5;
	param.p = 0.1;
	param.nr_weight = 0;
	param.weight_label = nullptr;
	param.weight = nullptr;

	svm_model* model;

	// train
	std::cout << "Now training..." << std::endl;
	model = svm_train(&prob, &param);
	std::cout << "Training complete !" << std::endl;

	// save
	std::cout << "Save model..." << std::endl;
	std::string model_path = dir_o + model_name;
	svm_save_model(model_path.c_str(), model);

	// test
	std::vector<svm_node*> test;
	set_test_data<n_feat>(test, test_path);
	predict_and_save_result<n_feat>(dir_o + "result.json", model, test);

	svm_free_and_destroy_model(&model);
	delete[] prob.x;
	delete[] prob.y;

	system("pause");
	return EXIT_SUCCESS;
}