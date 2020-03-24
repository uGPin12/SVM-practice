#include "dataIO.h"
#include "itkImageIO.h"

#include "util.h"
#include "common.h"

int main(int argc, char* argv[]) {

	if (argc != 2) {
		std::cout << "Usage: " << argv[0]
			<< "input_info.ini\n"
			<< "Hit any key to exit..." << std::endl;
		system("pause");
		return EXIT_FAILURE;
	}

	input_info info(argv[1]);
	
	std::vector<std::size_t> group;
	if (info.use_leave_one_out)
	{
		for (std::size_t cv = 0; cv < info.n_group; cv++)
		{
			group.push_back(cv + 1);
		}
	}
	else
	{
		std::cerr << "This code supports only LOOCV now.\n"
			<< "Hit any key to exit..." << std::endl;
		system("pause");
		return EXIT_FAILURE;
	}

	std::vector<std::string> feat_name;
	read_list<std::string>(feat_name, info.feat_txt);

	for (std::size_t cv_loop = 0; cv_loop < group.size(); cv_loop++) {
		std::string group_id = std::to_string(cv_loop + 1);
		std::cout << "Group" << group_id << " Start." << std::endl;

		svm_model* model;
		if (info.use_exist_model)
		{
			std::cout << "------Load model------" << std::endl;
			std::string dir_model = info.dir_o + info.param + "/group" + group_id + "/";
			std::string model_path = dir_model + "model";
			model = svm_load_model(dir_model.c_str());
		}
		else
		{
			std::vector<std::string> train_name;
			read_list<std::string>(train_name, info.name_txt + "group" + group_id + "/train_names.txt");
			
			std::vector<std::vector<std::vector<input_type>>>	train_X(train_name.size());
			std::vector<std::vector<mask_type>>					train_M(train_name.size());
			std::vector<std::vector<label_type>>				train_Y(train_name.size());

			std::vector<ImageIO<2>> train_mhd_X(train_name.size());
			std::vector<ImageIO<2>> train_mhd_M(train_name.size());
			std::vector<ImageIO<2>> train_mhd_Y(train_name.size());
			std::cout << "------Load train data------" << std::endl;
			for (std::size_t i_case = 0; i_case < train_name.size(); i_case++)
			{
				train_X[i_case].resize(feat_name.size());
				for (std::size_t i_feat = 0; i_feat < feat_name.size(); i_feat++)
				{
					train_mhd_X[i_case].Read(train_X[i_case][i_feat], info.dir_i + feat_name[i_feat] + train_name[i_case]);
				}
				train_mhd_Y[i_case].Read(train_Y[i_case], info.dir_tumor + train_name[i_case]);
				train_mhd_M[i_case].Read(train_M[i_case], info.dir_mask + train_name[i_case]);
			}
			
			normalization(train_X, train_M);
			
			svm_problem prob;
			set_svm_input(prob, train_X, train_Y, train_M, info.obj, info.bkg);
			svm_parameter param;
			info.set_param(param);

			std::cout << "------Training start------" << std::endl;
			model = svm_train(&prob, &param);
			std::cout << "------Training complete------" << std::endl;

			std::cout << "------Save model------" << std::endl;
			std::string dir_model = info.dir_o + info.param + "/group" + group_id + "/";
			mkdir_boost(dir_model);
			std::string model_path = dir_model + "model";
			svm_save_model(model_path.c_str(), model);

			delete[] prob.x;
			delete[] prob.y;
		}

		std::vector<std::string> test_name;
		read_list<std::string>(test_name, info.name_txt + "/group" + group_id + "/test_names.txt");

		std::cout << "------Load test data------" << std::endl;
		std::vector<std::vector<std::vector<input_type>>>	test_X(test_name.size());
		std::vector<std::vector<mask_type>>					test_M(test_name.size());
		
		std::vector<ImageIO<2>> test_mhd_X(test_name.size());
		std::vector<ImageIO<2>> test_mhd_M(test_name.size());
		for (std::size_t j_case = 0; j_case < test_name.size(); j_case++)
		{
			test_X[j_case].resize(feat_name.size());
			for (std::size_t j_feat = 0; j_feat < feat_name.size(); j_feat++)
			{
				test_mhd_X[j_case].Read(test_X[j_case][j_feat], info.dir_i + feat_name[j_feat] + test_name[j_case]);
			}
			test_mhd_M[j_case].Read(test_M[j_case], info.dir_mask + test_name[j_case]);
		}

		normalization(test_X, test_M);

		std::cout << "------Predicting start------" << std::endl;

		std::vector<std::vector<label_type>> labelO(test_name.size());
		for (std::size_t k_case = 0; k_case < test_name.size(); k_case++)
		{
			labelO[k_case].resize(test_M[k_case].size(), 0);
			for (std::size_t idx = 0; idx < labelO[k_case].size(); idx++)
			{
				if (test_M[k_case][idx] != 0)
				{
					svm_node* test;
					test = new svm_node[feat_name.size() + 1];
					for (std::size_t k_feat = 0; k_feat < feat_name.size(); k_feat++)
					{
						test[k_feat].index = k_feat + 1;
						test[k_feat].value = test_X[k_case][k_feat][idx];
					}
					test[feat_name.size()].index = -1;

					const int label = static_cast<int>(svm_predict(model, test));

					if (label == 1)
					{
						labelO[k_case][idx] = 1;
					}
					else
					{
						labelO[k_case][idx] = 0;
					}
				}
			}
		}

		std::cout << "------Predicting complete------" << std::endl;

		svm_free_and_destroy_model(&model);

		std::string dir_ext = info.dir_o + info.param + "/extraction/";
		mkdir_boost(dir_ext);
		for (std::size_t l_case = 0; l_case < test_name.size(); l_case++)
		{
			std::vector<unsigned int> siz = { test_mhd_X[l_case].Size(0), test_mhd_X[l_case].Size(1) };
			std::vector<double> spacing = { 1.0, 1.0, 1.0 };
			save_vector(dir_ext + test_name[l_case], labelO[l_case], siz, spacing, true);
		}

		std::cout << std::endl;
	}

	system("pause");
	return 0;

}
