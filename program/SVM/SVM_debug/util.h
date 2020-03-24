#pragma once

#include <string>
#include <iostream>


#include <boost/filesystem.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <boost/format.hpp>

#include "svm.h"


namespace fs = boost::filesystem;

// boostを用いたディレクトリ作成(エラーチェック付き)
void mkdir_boost(std::string dir)
{
	const fs::path path(dir);
	if (fs::exists(dir)) {
		return;
	}

	boost::system::error_code error;
	const bool result = fs::create_directories(path, error);
	if (!result || error) {
		std::cerr << "Fail to create dictionary : " << dir
			<< "\n Hit any key to exit..."
			<< std::endl;
		system("pause");
	}
}

// boostを用いたパラメータ読み込み
template<class RET>
RET read_param_boost(std::string path, std::string section, std::string key)
{
	boost::property_tree::ptree pt;
	boost::property_tree::read_ini(path, pt);

	if (boost::optional<RET> ret = pt.get_optional<RET>(section + "." + key)) {
		return ret.get();
	}
	else {
		std::cerr << key << " is nothing "
			<< "\n Hit any key to exit..."
			<< std::endl;
		system("pause");
		exit(EXIT_FAILURE);
	}

}

void myTrim(std::string& s)
{
	boost::algorithm::trim(s);
}

template<std::size_t n_feat>
void read_data_as_str_vector(std::vector<std::vector<std::string>>& data , std::string data_path) {
	std::ifstream ifs(data_path);
	if (!ifs) {
		std::cerr
			<< "Cannot open the file: "
			<< data_path << std::endl;
		system("pause");
		std::exit(EXIT_FAILURE);
	}

	std::string buf;
	std::vector<std::string> buf_str;
	while (std::getline(ifs, buf))
	{
		// The line including '#' is comment.
		if (buf.find('#') == std::string::npos) {
			boost::algorithm::split(buf_str, buf, boost::is_any_of(","));
			boost::for_each(buf_str, myTrim);
			if (buf_str.size() != n_feat) {
				std::cerr
					<< "Error : Inappropriate input, check "
					<< data_path << std::endl;
				system("pause");
				std::exit(EXIT_FAILURE);
			}

			data.push_back(buf_str);

		}
	}
}

template<std::size_t n_feat>
void set_svm_input(svm_problem& prob, std::string input_path, std::string label_path)
{
	std::vector<std::vector<std::string>> data;
	read_data_as_str_vector<n_feat>(data, input_path);

	std::vector<std::vector<std::string>> label;
	read_data_as_str_vector<n_feat>(label, label_path);

	if (label[0].size() != 1) {
		std::cerr
			<< "Error : label value must be scalar, check "
			<< label_path << std::endl;
		system("pause");
		std::exit(EXIT_FAILURE);
	}

	if (data.size() != label.size()) {
		std::cerr
			<< "Error : The number of data must be same as that of label, check "
			<< input_path << "and" << label_path << std::endl;
		system("pause");
		std::exit(EXIT_FAILURE);
	}

	std::size_t n_data = data.size();

	prob.l = static_cast<int> (n_data);
	prob.y = new double[n_data]; // Label 

	svm_node* prob_vec;
	prob_vec = new svm_node[n_data * (n_feat + 1)];
	prob.x = new svm_node* [prob.l];

	// Set libsvm input
	for (std::size_t i_data = 0; i_data < n_data; i_data++) {
		prob.y[i_data] = std::stod(label[i_data][0]);
		prob.x[i_data] = prob_vec + i_data * (n_feat + 1);
		for (std::size_t i_feat = 0; i_feat < n_feat; i_feat++) {
			prob.x[i_data][i_feat].index = static_cast<int> (i_feat + 1);
			prob.x[i_data][i_feat].value = std::stod(data[i_data][i_feat]);
		}
		prob.x[i_data][n_feat].index = -1;
	}
}

template<std::size_t n_feat>
void set_test_data(std::vector<svm_node*>& test, std::string test_path) {

	std::vector<std::vector<std::string>> data;
	read_data_as_str_vector<n_feat>(data, test_path);
	
	svm_node* buf_node = new svm_node[n_feat + 1];
	for (std::size_t i_data = 0; i_data < data.size(); i_data++) {
		for (std::size_t i_feat = 0; i_feat < n_feat; i_feat++) {
			buf_node[i_feat].index = static_cast<int> (i_feat + 1);
			buf_node[i_feat].value = std::stod(data[i_data][i_feat]);
		}
		buf_node[n_feat].index = -1;

		test.push_back(buf_node);
	}
	
}

template<std::size_t n_feat>
void predict_and_save_result(std::string path, const svm_model* model, std::vector<svm_node*>& test) {

	using namespace boost::property_tree;

	ptree pt;
	ptree test_data; 
	ptree predict;
	for (std::size_t i_data = 0; i_data < test.size(); i_data++) {
		std::size_t i_feat = 0;
		ptree one_data;
		while (test[i_data][i_feat].index != -1)
		{
			ptree test_value;
			test_value.put_value(test[i_data][i_feat].value);
			one_data.push_back(std::make_pair("", test_value));
			i_feat++;
		}

		test_data.push_back(std::make_pair("", one_data));
		ptree predict_value;
		predict_value.put_value(svm_predict(model, test[i_data]));
		predict.push_back(std::make_pair("", predict_value));

	}
	pt.add_child("test", test_data);
	pt.add_child("predict", predict);
	write_json(path, pt);

}
