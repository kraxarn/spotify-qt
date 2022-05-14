#include "commandline/processor.hpp"
#include "commandline/args.hpp"
#include "lib/log.hpp"
#include "ipc/client.hpp"

#include <QThread>

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
	}
	else if (parser.isSet(ARG_PREVIOUS_TRACK))
	{
		client.send(ARG_PREVIOUS_TRACK);
	}
	else if (parser.isSet(ARG_NEXT_TRACK))
	{
		client.send(ARG_NEXT_TRACK);
	}
	else
	{
		return false;
	}

	auto running = true;
	auto callback = [&running](const QString &response)
	{
		if (!response.startsWith("QLocalSocket:"))
		{
			std::cout << response.toStdString() << std::endl;
		}
		running = false;
	};

	client.setOnSuccess(callback);
	client.setOnError(callback);

	// TODO: Maybe not a very good way to wait for response?
	while (running)
	{
		QCoreApplication::processEvents();
		QThread::msleep(10);
	}

	return true;
}
