#include "median.h"
#include "dilation.h"
#include "erosion.h"
#include "opening.h"
#include "closing.h"
#include "sharpening_4N.h"
#include "sharpening_8N.h"
#include "convergence.h"
#include "LoG.h"
#include "identity.h"

#include "util.h"
#include "dataIO.h"
#include "itkImageIO.h"

int main(int argc, char* argv[])
{
	if (argc != 2) {
		std::cerr
			<< "Usage: " << argv[0]
			<< " input_info.ini" << std::endl;
		system("pause");
		return EXIT_FAILURE;
	}
	std::string path = argv[1];

	std::string dir_i = read_param_boost<std::string>(path, "path", "dir_i");
	std::string dir_o = read_param_boost<std::string>(path, "path", "dir_o");
	std::string dir_mask = read_param_boost<std::string>(path, "path", "dir_mask");
	std::string dir_name = read_param_boost<std::string>(path, "path", "dir_name");

	std::vector<std::string> case_names;
	read_name_list(case_names, dir_name + "case_names.txt");

	std::vector<std::vector<output_type>> imgO;
	std::vector<std::string> feat_names;
	std::vector<input_type> imgI;
	std::vector<mask_type> mask;
	ImageIO<2> mhdI;
	ImageIO<2> mhdM;

	std::ofstream ofs(dir_o + "feature_list.txt");
	for (std::size_t i_case = 0; i_case < case_names.size(); i_case++) {
		std::cout << "\n------Case : " << case_names[i_case] << " start------" << std::endl;

		imgO.erase(imgO.begin(), imgO.end());

		mhdI.Read(imgI, dir_i + case_names[i_case]);
		mhdM.Read(mask, dir_mask + case_names[i_case]);
		std::vector<unsigned int> siz = { mhdI.Size(0), mhdI.Size(1) };

		std::vector<output_type> buf(imgI.size());
		identity(buf, imgI, siz);
		imgO.push_back(buf);
		feat_names.push_back("org/");
		mkdir_boost(dir_o + "org/");
		std::cout << "------Idenfity map : complete------" << std::endl;

		median(buf, imgI, mask, siz, 3);
		imgO.push_back(buf);
		feat_names.push_back("median/");
		mkdir_boost(dir_o + "median/");
		std::cout << "------Median : complete------" << std::endl;

		closing(buf, imgI, mask, siz, 3);
		imgO.push_back(buf);
		feat_names.push_back("closing/");
		mkdir_boost(dir_o + "closing/");
		std::cout << "------Closing : complete------" << std::endl;

		opening(buf, imgI, mask, siz, 3);
		imgO.push_back(buf);
		feat_names.push_back("opening/");
		mkdir_boost(dir_o + "opening/");
		std::cout << "------opening : complete------" << std::endl;

		LoG(buf, imgI, mask, siz, 0.5);
		imgO.push_back(buf);
		feat_names.push_back("LoG/");
		mkdir_boost(dir_o + "LoG/");
		std::cout << "------LoG : complete------" << std::endl;

		sharpening_4N(buf, imgI, mask, siz, 1);
		imgO.push_back(buf);
		feat_names.push_back("sharpening_4N/");
		mkdir_boost(dir_o + "sharpening_4N/");
		std::cout << "------sharpening_4N : complete------" << std::endl;

		sharpening_8N(buf, imgI, mask, siz, 1);
		imgO.push_back(buf);
		feat_names.push_back("sharpening_8N/");
		mkdir_boost(dir_o + "sharpening_8N/");
		std::cout << "------sharpening_4N : complete------" << std::endl;

		sobel(buf, imgI, mask, siz, 2);
		imgO.push_back(buf);
		feat_names.push_back("sobel_xy/");
		mkdir_boost(dir_o + "sobel_xy/");
		std::cout << "------sobel_xy : complete------" << std::endl;

		convergence(buf, imgI, mask, siz, 100, 1, 10, 1);
		imgO.push_back(buf);
		feat_names.push_back("convergence/");
		mkdir_boost(dir_o + "convergence/");
		std::cout << "------convergence : complete------" << std::endl;

		for (std::size_t i_feat = 0; i_feat < imgO.size(); i_feat++) {
			mhdI.Write(imgO[i_feat], dir_o + feat_names[i_feat] + case_names[i_case], true);
			if (i_case == 0) {
				ofs << feat_names[i_feat] << "\n";
			}
		}
	}

	ofs.close();
	system("pause");
	return 0;
	
}