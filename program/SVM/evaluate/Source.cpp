#include <iostream>
#include <fstream>

#include "itkImageIO.h"
#include "common.h"
#include "dataIO.h"
#include "util.h"

int main(int argc, char* argv[]) {

	if (argc != 2) {
		std::cout << "Usage: " << argv[0]
			<< "evaluate.ini\n"
			<< "Hit any key to exit..." << std::endl;
		system("pause");
		return EXIT_FAILURE;
	}

	std::string path = argv[1];

	std::string param		= read_param_boost<std::string>(path, "param", "param");
	std::string dir_i		= read_param_boost<std::string>(path, "path", "dir_i");
	std::string dir_tumor	= read_param_boost<std::string>(path, "path", "dir_tumor");
	std::string dir_mask	= read_param_boost<std::string>(path, "path", "dir_mask");
	std::string case_list	= read_param_boost<std::string>(path, "path", "case_list");

	std::vector<std::string> case_names;
	read_list<std::string>(case_names, case_list);
	
	std::vector<std::vector<seg_type>>		seg_meta(case_names.size());
	std::vector<std::vector<mask_type>>		mask(case_names.size());
	std::vector<std::vector<label_type>>	answer(case_names.size());
	std::vector<ImageIO<2>> seg_mhd(case_names.size());
	std::vector<ImageIO<2>> mask_mhd(case_names.size());
	std::vector<ImageIO<2>> ans_mhd(case_names.size());
	for (std::size_t i_case = 0; i_case < case_names.size(); i_case++) {
		seg_mhd[i_case].Read(seg_meta[i_case], dir_i + param + "/extraction/" + case_names[i_case]);
		mask_mhd[i_case].Read(mask[i_case], dir_mask+ case_names[i_case]);
		ans_mhd[i_case].Read(answer[i_case], dir_tumor + case_names[i_case]);
	}

	std::vector<double> ji(case_names.size(), 0);
	std::vector<double> tp(case_names.size(), 0);
	std::vector<double> fp(case_names.size(), 0);
	std::vector<double> fn(case_names.size(), 0);

	enum eval
	{
		TP = 1,
		FP,
		FN
	};

	std::cout << "\n------Evaluation start------" << std::endl;
	std::vector<std::vector<label_type>> labelO(case_names.size());
	for (std::size_t i_case = 0; i_case < case_names.size(); i_case++) {
		labelO[i_case].resize(seg_meta[i_case].size(), 0);
		for (std::size_t i = 0; i < seg_meta[i_case].size(); i++) {
			if (mask[i_case][i] != 0) {
				if (seg_meta[i_case][i] != 0 && answer[i_case][i] != 0) {
					tp[i_case]++;
					labelO[i_case][i] = TP;
				}
				else if (seg_meta[i_case][i] != 0 && answer[i_case][i] == 0) {
					fp[i_case]++;
					labelO[i_case][i] = FP;
				}
				else if (seg_meta[i_case][i] == 0 && answer[i_case][i] != 0) {
					fn[i_case]++;
					labelO[i_case][i] = FN;
				}
			}
		}

		ji[i_case] = tp[i_case] / (tp[i_case] + fp[i_case] + fn[i_case]);
	}

	std::string dir_eval = dir_i + param + "/evaluation/";
	mkdir_boost(dir_eval); mkdir_boost(dir_eval + "extraction/");
	for (std::size_t j_case = 0; j_case < case_names.size(); j_case++) {
		save_vector(dir_eval + "extraction/"+case_names[j_case], labelO[j_case], { seg_mhd[j_case].Size(0),seg_mhd[j_case].Size(1) }, { 1,1 }, true);
	}
	std::ofstream ofs(dir_eval + "evaluation.csv");
	ofs << ","
		<< "JI" << ","
		<< "TP" << ","
		<< "FP" << ","
		<< "FN" 
		<< std::endl;
	for (std::size_t i_case = 0; i_case < case_names.size(); i_case++) {
		ofs << "," 
			<< ji[i_case] << "," 
			<< tp[i_case] << "," 
			<< fp[i_case] << ","
			<< fn[i_case] 
			<< std::endl;
	}
	
	ofs << "average,"
		<< std::accumulate(ji.begin(),ji.end(),0)/static_cast<double>(ji.size()) << ","
		<< std::accumulate(tp.begin(),tp.end(),0)/static_cast<double>(tp.size()) << ","
		<< std::accumulate(fp.begin(),fp.end(),0)/static_cast<double>(fp.size()) << ","
		<< std::accumulate(fn.begin(),fn.end(),0)/static_cast<double>(fn.size()) 
		<< std::endl;

	std::cout << "\n------Evaluation complete------" << std::endl;
	system("pause");
	return 0;
}