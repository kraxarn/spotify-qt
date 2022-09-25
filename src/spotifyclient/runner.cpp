#include "spotifyclient/runner.hpp"
#include "mainwindow.hpp"
#include "dialog/passwordentry.hpp"

#ifdef USE_KEYCHAIN
#include "util/keychain.hpp"
#endif

std::vector<lib::log_message> SpotifyClient::Runner::log;

SpotifyClient::Runner::Runner(const lib::settings &settings,
	const lib::paths &paths, QWidget *parent)
	: QObject(parent),
	parentWidget(parent),
	settings(settings),
	paths(paths)
{
	path = QString::fromStdString(settings.spotify.path);
	process = new QProcess(parent);
	clientType = SpotifyClient::Helper::clientType(path);
}

SpotifyClient::Runner::~Runner()
{
	if (process != nullptr)
	{
		process->close();
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
	if (clientType == lib::client_type::none)
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

	// If using global config, just start
	if (settings.spotify.global_config && clientType == lib::client_type::spotifyd)
	{
		process->start(path, QStringList({QStringLiteral("--no-daemon")}));
		emit statusChanged({});
		return;
	}

	// Check if username exists
	const auto username = QString::fromStdString(settings.spotify.username);
	if (username.isEmpty())
	{
		emit statusChanged(QStringLiteral("No username provided"));
		return;
	}

	// Try to get password
	QString password;
#ifdef USE_KEYCHAIN
	password = Keychain::getPassword(username);
#endif

	if (password.isEmpty())
	{
		auto *dialog = new Dialog::PasswordEntry(this, parentWidget);
		dialog->open(username);
	}
	else
	{
		start(username, password);
	}
}

void SpotifyClient::Runner::start(const QString &username, const QString &password)
{
	if (password.isEmpty())
	{
		emit statusChanged(QStringLiteral("No password provided"));
		return;
	}

	// Common arguments
	QStringList arguments({
		"--bitrate", QString::number(static_cast<int>(settings.spotify.bitrate)),
		"--username", username,
		"--password", password
	});

	// librespot specific
	if (clientType == lib::client_type::librespot)
	{
		arguments.append({
			"--name", QString("%1 (librespot)").arg(APP_NAME),
			"--initial-volume", "100",
			"--cache", QString::fromStdString((paths.cache() / "librespot").string()),
		});

		if (SpotifyClient::Helper::supportsAutoplay(path))
		{
			arguments.append(QStringLiteral("--autoplay"));
		}
	}
	else if (clientType == lib::client_type::spotifyd)
	{
		arguments.append({
			"--no-daemon",
			"--device-name", QString("%1 (spotifyd)").arg(APP_NAME),
		});
	}

	auto backend = QString::fromStdString(settings.spotify.backend);
	if (!backend.isEmpty())
	{
		arguments.append({
			"--backend", backend
		});
	}

	if (clientType == lib::client_type::librespot && settings.spotify.disable_discovery)
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

	QProcess::connect(process, &QProcess::readyReadStandardOutput,
		this, &Runner::onReadyReadOutput);

	QProcess::connect(process, &QProcess::readyReadStandardError,
		this, &Runner::onReadyReadError);

	QProcess::connect(process, &QProcess::started,
		this, &Runner::onStarted);

	QProcess::connect(process, &QProcess::errorOccurred,
		this, &Runner::onErrorOccurred);

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

void SpotifyClient::Runner::logOutput(const QByteArray &output, lib::log_type logType) const
{
	for (auto &line: QString(output).split('\n'))
	{
		if (line.isEmpty())
		{
			continue;
		}

		log.emplace_back(lib::date_time::now(), logType, line.toStdString());

#ifdef USE_KEYCHAIN
		if (line.contains(QStringLiteral("Bad credentials")))
		{
			const auto username = QString::fromStdString(settings.spotify.username);
			if (Keychain::clearPassword(username))
			{
				lib::log::warn("Bad credentials, cleared saved password");
			}
		}
#endif
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
				i >= args.size() - 1 ? " " : ""));
	}
	return result;
}

void SpotifyClient::Runner::onReadyReadOutput() const
{
	logOutput(process->readAllStandardOutput(), lib::log_type::information);
}

void SpotifyClient::Runner::onReadyReadError() const
{
	logOutput(process->readAllStandardError(), lib::log_type::error);
}

void SpotifyClient::Runner::onStarted()
{
	emit statusChanged({});
}

void SpotifyClient::Runner::onErrorOccurred(QProcess::ProcessError error)
{
	emit statusChanged(QString("Error %1").arg(error));
}

auto SpotifyClient::Runner::getLog() -> const std::vector<lib::log_message> &
{
	return log;
}
