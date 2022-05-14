#include "commandline/parser.hpp"
#include "commandline/args.hpp"

CommandLine::Parser::Parser(const QCoreApplication &app)
{
	addVersionOption();
	addHelpOption();
	addOptions(options());

	process(app);
}

auto CommandLine::Parser::options() -> QList<QCommandLineOption>
{
	return {
		{
			ARG_ENABLE_DEV,
			QStringLiteral("Enable developer mode for troubleshooting issues."),
		},
		{
			ARG_FORCE_SETUP,
			QStringLiteral("Allows providing new Spotify credentials."),
		},
		{
			ARG_LIST_PATHS,
			QStringLiteral("Print paths for config file and cache."),
		},
		{
			ARG_DISABLE_IPC,
			QStringLiteral("Allow multiple instances, disables playback options."),
		},
		// Playback options
		{
			ARG_PLAY_PAUSE,
			QStringLiteral("Play, or pause, current track.")
		},
		{
			ARG_PREVIOUS_TRACK,
			QStringLiteral("Go to the previous track."),
		},
		{
			ARG_NEXT_TRACK,
			QStringLiteral("Go to the next track."),
		},
	};
}
