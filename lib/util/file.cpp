#include "file.hpp"

bool lib::file::exists(const std::string &path)
{
	std::ifstream file(path);
	return file.good();
}
