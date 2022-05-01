#include "handler.hpp"
#include "lib/log.hpp"

auto Ipc::Handler::process(const QCommandLineParser &parser) -> bool
{
	if (parser.isSet("dev"))
	{
		lib::developer_mode::enabled = true;
		return false;
	}

	return false;
}
