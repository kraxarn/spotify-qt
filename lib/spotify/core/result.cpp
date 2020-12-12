#include "result.hpp"

using namespace lib::spt::core;

result result::ok()
{
	result result;
	result.success = true;
	return result;
}

result result::err(const std::string &message)
{
	result result;
	result.success = false;
	result.message = std::string(message);
	return result;
}

