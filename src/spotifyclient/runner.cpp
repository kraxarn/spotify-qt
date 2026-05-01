#include "spotifyclient/runner.hpp"
#include "spotifyclient/helper.hpp"

#include "lib/enums.hpp"
#include "lib/log.hpp"
#include "util/url.hpp"

#include <QSysInfo>

QList<LogMessage> SpotifyClient::Runner::log = QList<LogMessage>();

SpotifyClient::Runner::Runner(const lib::settings &settings,
	const lib::paths &paths, QWidget *parent)
	: QObject(parent),
	parentWidget(parent),
	settings(settings),
	paths(paths)
{
	path = QString::fromStdString(settings.spotify.path);
	process = new QProcess(parent);

	connect(process, &QProcess::readyReadStandardOutput,
		this, &Runner::onReadyReadOutput);

	connect(process, &QProcess::readyReadStandardError,
		this, &Runner::onReadyReadError);

	connect(process, &QProcess::started,
		this, &Runner::onStarted);

	connect(process, &QProcess::errorOccurred,
		this, &Runner::onErrorOccurred);
}

SpotifyClient::Runner::~Runner()
{
	if (process != nullptr)
	{
		if (process->disconnect())
		{
			lib::log::debug("Disconnected events from client process");
		}

		process->terminate();
		process->waitForFinished();
	}
}

void SpotifyClient::Runner::start()
{
	// Don't start if already running
	if (!settings.spotify.always_start && isRunning())
	{
		emit statusChanged({});
		return;
	}

	// Check if empty
	if (path.isEmpty())
	{
		emit statusChanged(QStringLiteral("Client path is empty or invalid"));
		return;
	}

	// Check if path exists
	QFileInfo info(path);
	if (!info.exists())
	{
		emit statusChanged(QStringLiteral("Client path does not exist"));
		return;
	}

	// Common arguments
	QStringList arguments({
		"--bitrate", QString::number(static_cast<int>(settings.spotify.bitrate)),
	});

	// Check if not logged in
	if (!isLoggedIn())
	{
		if (!Helper::getOAuthSupport(path))
		{
			emit statusChanged(QStringLiteral("Client unsupported, please upgrade and try again"));
			return;
		}

		arguments.append(QStringLiteral("--enable-oauth"));
	}

	const auto initialVolume = QString::number(settings.spotify.volume);

	const auto deviceName = QStringLiteral("%1@%2")
		.arg(APP_NAME, QSysInfo::machineHostName());

	arguments.append({
		QStringLiteral("--name"), deviceName,
		QStringLiteral("--initial-volume"), initialVolume,
		QStringLiteral("--cache"), QString::fromStdString(getCachePath().string()),
		QStringLiteral("--autoplay"), settings.spotify.autoplay ? QStringLiteral("on") : QStringLiteral("off"),
	});

	auto backend = QString::fromStdString(settings.spotify.backend);
	if (!backend.isEmpty())
	{
		arguments.append({
			"--backend", backend
		});
	}

	if (settings.spotify.disable_discovery)
	{
		arguments.append("--disable-discovery");
	}

	const auto deviceType = settings.spotify.device_type;
	if (deviceType != lib::device_type::unknown)
	{
		const auto deviceTypeString = lib::enums<lib::device_type>::to_string(deviceType);
		if (!deviceTypeString.empty())
		{
			arguments.append({
				QStringLiteral("--device-type"),
				QString::fromStdString(deviceTypeString).remove(' '),
			});
		}
	}

	auto additional_arguments = QString::fromStdString(settings.spotify.additional_arguments);
	if (!additional_arguments.isEmpty())
	{
		arguments.append(additional_arguments.split(' '));
	}

	lib::log::debug("starting: {} {}", path.toStdString(),
		joinArgs(arguments).toStdString());

	process->start(path, arguments);
}

auto SpotifyClient::Runner::isRunning() const -> bool
{
	return process == nullptr
		? SpotifyClient::Helper::running(path)
		: process->isOpen();
}

void SpotifyClient::Runner::logOutput(const QByteArray &output, QtMsgType logType)
{
	for (const QString &line : QString(output).split(QChar::fromLatin1('\n')))
	{
		if (line.isEmpty())
		{
			continue;
		}

		log.append({logType, line});

		const auto urlIndex = line.indexOf(QStringLiteral("https://accounts.spotify.com/authorize"));
		if (urlIndex >= 0)
		{
			const auto url = line.right(line.length() - urlIndex);
			auto *parent = qobject_cast<QWidget*>(QObject::parent());
			Url::open(url, LinkType::Web, parent);
		}

		if (line.contains(QStringLiteral("Bad credentials")))
		{
			emit statusChanged(QStringLiteral("Bad credentials, please try again"));

			if (resetCredentials())
			{
				lib::log::debug("Credentials reset");
			}
		}
	}
}

auto SpotifyClient::Runner::joinArgs(const QStringList &args) -> QString
{
	QString result;
	for (auto i = 0; i < args.size(); i++)
	{
		const auto &arg = args.at(i);
		result.append(QString("%1%2%1%3")
			.arg(arg.contains(' ') ? "\"" : "", arg,
				i < args.size() - 1 ? " " : ""));
	}
	return result;
}

auto SpotifyClient::Runner::getCachePath() const -> std::filesystem::path
{
	return paths.cache() / "librespot";
}

auto SpotifyClient::Runner::isLoggedIn() const -> bool
{
	const auto path = getCachePath() / "credentials.json";
	return std::filesystem::exists(path);
}

auto SpotifyClient::Runner::resetCredentials() const -> bool
{
	const auto path = getCachePath() / "credentials.json";
	return std::filesystem::remove(path);
}

void SpotifyClient::Runner::onReadyReadOutput()
{
	logOutput(process->readAllStandardOutput(), QtInfoMsg);
}

void SpotifyClient::Runner::onReadyReadError()
{
	logOutput(process->readAllStandardError(), QtCriticalMsg);
}

void SpotifyClient::Runner::onStarted()
{
	emit statusChanged({});
}

void SpotifyClient::Runner::onErrorOccurred(QProcess::ProcessError error)
{
	const auto message = Helper::processErrorToString(error);
	emit statusChanged(message);
}

auto SpotifyClient::Runner::getLog() -> const QList<LogMessage> &
{
	return log;
}
