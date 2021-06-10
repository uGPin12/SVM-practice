#pragma once

#include <string>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/optional.hpp>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "svm.h"
#include "common.h"

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

template<class DataType>
void read_list(std::vector<DataType>& list,std::string filepath)
{
	std::ifstream ifs(filepath);
	if (!ifs) {
		std::cerr << "Cannot open : "
			<< filepath
			<< "\nHit any key to exit..." << std::endl;
		system("pause");
		std::exit(EXIT_FAILURE);
	}

	DataType buf;
	while (std::getline(ifs, buf))
	{
		list.push_back(buf);
	}
}

void normalization(std::vector<std::vector<std::vector<input_type>>>& X, const std::vector<std::vector<mask_type>>& mask)
{
	for (std::size_t i_case = 0; i_case < X.size(); i_case++)
	{
		for (std::size_t i_feat = 0; i_feat < X[i_case].size(); i_feat++)
		{
			double sum = 0.0, ave = 0.0, var = 0.0, sigma = 0.0;
			auto data_num = 0;
			for (std::size_t i = 0; i < X[i_case][i_feat].size(); i++) 
			{
				if (mask[i_case][i] != 0)
				{
					sum += X[i_case][i_feat][i];
					data_num++;
				}
			}
			ave = static_cast<double>(sum / data_num);

			for (std::size_t j = 0; j < X[i_case][i_feat].size(); j++)
			{
				if (mask[i_case][j] == 1)
				{
					var += (X[i_case][i_feat][j] - ave) * (X[i_case][i_feat][j] - ave);
				}
			}

			var /= (static_cast<double>(data_num) - 1.0);
			sigma = std::sqrt(var);

			for (std::size_t k = 0; k < X[i_case][i_feat].size(); k++)
			{
				if (mask[i_case][k] != 0)
				{
					if (sigma != 0.0)
					{
						X[i_case][i_feat][k] = static_cast<double>(X[i_case][i_feat][k] - ave) / sigma;
					}
					else
					{
						X[i_case][i_feat][k] = static_cast<double>(X[i_case][i_feat][k] - ave) / FLT_MIN;
					}
					
				}
			}
		}
	}
}

class input_info
{
public:

	std::string param;
	std::string dir_i;
	std::string dir_o;
	std::string dir_tumor;
	std::string dir_mask;
	std::string feat_txt;
	std::string name_txt;

	int CV_group;
	int n_group;
	bool use_leave_one_out;
	bool use_exist_model;

	std::string kernel_type;
	int degree;
	double gamma;
	double coef0;
	double C;

	int obj;
	int bkg;

	input_info(const std::string& input_path)
	{
		Refer(input_path);
	}
	~input_info() {};

	void Refer(const std::string& input_path);
	void set_param(svm_parameter& param);

private:
};

void input_info::Refer(const std::string& input_path)
{
	param		= read_param_boost<std::string>(input_path, "param", "param");
	dir_i		= read_param_boost<std::string>(input_path, "path", "dir_i");
	dir_o		= read_param_boost<std::string>(input_path, "path", "dir_o");
	dir_tumor	= read_param_boost<std::string>(input_path, "path", "dir_tumor");
	dir_mask	= read_param_boost<std::string>(input_path, "path", "dir_mask");
	feat_txt	= read_param_boost<std::string>(input_path, "path", "feat_txt");
	name_txt	= read_param_boost<std::string>(input_path, "path", "name_txt");

	CV_group			= read_param_boost<int>(input_path, "param", "CV_group");
	n_group				= read_param_boost<int>(input_path, "param", "n_group");
	use_leave_one_out	= read_param_boost<bool>(input_path, "param", "use_leave_one_out");
	use_exist_model		= read_param_boost<bool>(input_path, "param", "use_exist_model");

	kernel_type = read_param_boost<std::string>(input_path, "param", "kernel_type");
	degree		= read_param_boost<int>(input_path, "param", "degree");
	gamma		= read_param_boost<double>(input_path, "param", "gamma");
	coef0		= read_param_boost<double>(input_path, "param", "coef0");
	C			= read_param_boost<double>(input_path, "param", "C");

	obj = read_param_boost<int>(input_path, "param", "obj");
	bkg = read_param_boost<int>(input_path, "param", "bkg");

}

void input_info::set_param(svm_parameter& param)
{
	if		(kernel_type == "liner")	param.kernel_type = LINEAR;
	else if (kernel_type == "poly")		param.kernel_type = POLY;
	else if (kernel_type == "rbf")		param.kernel_type = RBF;
	else if (kernel_type == "sigmoid")	param.kernel_type = SIGMOID;
	else
	{
		std::cerr << "Not defined for the specified kernel type.\n"
			<< "Hit any key to exit..." << std::endl;
		system("pause");
		std::exit(EXIT_FAILURE);
	}

	param.C			= C;
	param.gamma		= gamma;
	param.degree	= degree;
	param.coef0		= coef0;

	param.svm_type		= C_SVC;
	param.eps			= 1e-3;
	param.cache_size	= 100;
	param.probability	= 0;
	param.nu			= 0.5;
	param.p				= 0.1;
	param.nr_weight		= 0;
	param.weight_label	= nullptr;
	param.weight		= nullptr;
	param.shrinking		= 0;
}

void set_svm_input(
	svm_problem& prob,
	std::vector<std::vector<std::vector<input_type>>>& X, std::vector<std::vector<label_type>>& Y, std::vector<std::vector<mask_type>>& mask,
	int obj, int bkg
)
{
	long se	= static_cast<long>	(X[0][0].size());
	std::size_t	n_feat = X[0].size();

	std::vector<unsigned int> obj_idx, bkg_idx;
	long counter = 0;
	for (std::size_t i_case = 0; i_case < X.size(); i_case++)
	{
		for (std::size_t i = 0; i < se; i++)
		{
			if (mask[i_case][i] != 0)
			{
				if (Y[i_case][i] != 0)
				{
					obj_idx.push_back(counter);
				}
				else
				{
					bkg_idx.push_back(counter);
				}
			}
			counter++;
		}
	}

	std::random_shuffle(obj_idx.begin(), obj_idx.end());
	std::random_shuffle(bkg_idx.begin(), bkg_idx.end());
	
	svm_node* prob_vec = new svm_node[static_cast<std::size_t>((obj + bkg) * (n_feat + 1))];

	prob.l = obj + bkg;
	prob.x = new svm_node * [static_cast<std::size_t>(prob.l)];
	prob.y = new double[static_cast<std::size_t>(prob.l)];
	
	for (std::size_t i_obj = 0; i_obj < static_cast<std::size_t>(obj); i_obj++)
	{
		prob.y[i_obj] = 1.0;
		prob.x[i_obj] = prob_vec + i_obj * (n_feat + 1);
		for (std::size_t i_feat = 0; i_feat < n_feat; i_feat++)
		{
			prob.x[i_obj][i_feat].index = static_cast<int>(i_feat + 1);
			prob.x[i_obj][i_feat].value = static_cast<double>(
				X[static_cast<std::size_t>(obj_idx[i_obj] / se)][i_feat][static_cast<std::size_t>(obj_idx[i_obj] % se)]
				);
		}
		prob.x[i_obj][n_feat].index = -1;
	}

	for (std::size_t i_bkg = obj; i_bkg < static_cast<std::size_t>(prob.l); i_bkg++)
	{
		prob.y[i_bkg] = -1.0;
		prob.x[i_bkg] = prob_vec + i_bkg * (n_feat + 1);
		for (std::size_t j_feat = 0; j_feat < n_feat; j_feat++)
		{
			prob.x[i_bkg][j_feat].index = static_cast<int>(j_feat + 1);
			prob.x[i_bkg][j_feat].value = static_cast<double>(
				X[static_cast<std::size_t>(bkg_idx[i_bkg] / se)][j_feat][static_cast<std::size_t>(bkg_idx[i_bkg] % se)]
				);
		}
		prob.x[i_bkg][n_feat].index = -1;
	}
}

