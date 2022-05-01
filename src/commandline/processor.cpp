#include "commandline/processor.hpp"
#include "commandline/args.hpp"
#include "lib/log.hpp"

auto CommandLine::Processor::process(const QCommandLineParser &parser) -> bool
{
	if (parser.isSet(ARG_ENABLE_DEV))
	{
		lib::developer_mode::enabled = true;
		return false;
	}

	return false;
}
