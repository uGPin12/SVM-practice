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
			<< "Hit any key to exit..." << std::endl;
		system("pause");
		std::exit(EXIT_FAILURE);
	}

	DataType buf;
	while (std::getline(ifs, buf))
	{
		list.push_back(buf);
	}
}
