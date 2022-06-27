#include "commandline/processor.hpp"
#include "commandline/args.hpp"
#include "lib/log.hpp"

#ifdef USE_DBUS
#include "mediaplayer/client.hpp"
#endif

#include <QtDebug>

auto CommandLine::Processor::process(const QCommandLineParser &parser) -> bool
{
	if (parser.isSet(ARG_ENABLE_DEV))
	{
		lib::developer_mode::enabled = true;
		return false;
	}

#ifdef USE_DBUS
	MediaPlayer::Client client;

	if (parser.isSet(ARG_PLAY_PAUSE))
	{
		client.playPause();
	}
	else if (parser.isSet(ARG_PREVIOUS_TRACK))
	{
		client.previousTrack();
	}
	else if (parser.isSet(ARG_NEXT_TRACK))
	{
		client.nextTrack();
	}
	else if (parser.isSet(ARG_METADATA))
	{
		qInfo().noquote() << client.metadata();
	}
	else
	{
		return false;
	}
#endif

	return true;
}
