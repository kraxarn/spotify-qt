#include "commandline/processor.hpp"
#include "commandline/args.hpp"
#include "lib/log.hpp"
#include "ipc/client.hpp"

auto CommandLine::Processor::process(const QCommandLineParser &parser) -> bool
{
	if (parser.isSet(ARG_ENABLE_DEV))
	{
		lib::developer_mode::enabled = true;
		return false;
	}

	Ipc::Client client(nullptr);

	if (parser.isSet(ARG_PLAY_PAUSE))
	{
		client.send(ARG_PLAY_PAUSE);
		return true;
	}

	return false;
}
