#include "file.hpp"

bool lib::file::exists(const std::string &path)
{
	std::ifstream file(path);
	return file.good();
}

std::string lib::file::read_all(std::ifstream &file)
{
	return std::string(std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>());
}
