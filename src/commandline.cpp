#include "commandline.hpp"

CommandLine::CommandLine(const QCoreApplication &app)
{
	addVersionOption();
	addHelpOption();
	addOptions(options());

	process(app);
}

auto CommandLine::options() -> QList<QCommandLineOption>
{
	return {
		{
			QStringLiteral("dev"),
			QStringLiteral("Enable developer mode for troubleshooting issues."),
		},
		{
			QStringLiteral("reset-credentials"),
			QStringLiteral("Allows providing new Spotify credentials."),
		},
		{
			QStringLiteral("paths"),
			QStringLiteral("Print paths for config file and cache."),
		},
		{
			QStringLiteral("multiple-instances"),
			QStringLiteral("Allow multiple instances, disables playback options."),
		},
		// Playback options
		{
			QStringLiteral("play-pause"),
			QStringLiteral("Play, or pause, current track.")
		},
		{
			QStringLiteral("previous"),
			QStringLiteral("Go to the previous track"),
		},
		{
			QStringLiteral("next"),
			QStringLiteral("Go to the next track"),
		},
	};
}
