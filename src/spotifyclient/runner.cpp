#include "spotifyclient/runner.hpp"
#include "mainwindow.hpp"

QList<QPair<QDateTime, QString>> SpotifyClient::Runner::log;

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

auto SpotifyClient::Runner::start() -> QString
{
	// Don't start if already running
	if (!settings.spotify.always_start && isRunning())
	{
		return {};
	}

	// Check if empty
	if (clientType == lib::client_type::none)
	{
		return "path is empty or invalid";
	}

	// Check if path exists
	QFileInfo info(path);
	if (!info.exists())
	{
		return "file in path does not exist";
	}

	// If using global config, just start
	if (settings.spotify.global_config && clientType == lib::client_type::spotifyd)
	{
		process->start(path, QStringList());
		return {};
	}

	// Check if username exists
	auto username = QString::fromStdString(settings.spotify.username);
	if (username.isEmpty())
	{
		return "no username provided";
	}

	// Get password from keyring if set
	QString password;
#ifdef USE_DBUS
	KWallet keyring(username);
	if (settings.spotify.keyring_password && keyring.unlock())
	{
		password = keyring.readPassword();
	}
#endif

	// Ask for password
	if (password.isEmpty())
	{
		password = QInputDialog::getText(parentWidget,
			"Enter password",
			QString("Enter password for Spotify user \"%1\":").arg(username),
			QLineEdit::Password);

		if (password.isEmpty())
		{
			return "no password provided";
		}

#ifdef USE_DBUS
		if (settings.spotify.keyring_password)
		{
			keyring.writePassword(password);
		}
#endif
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
			"--autoplay",
			"--cache", QString::fromStdString(paths.cache() / "librespot"),
		});
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

	QProcess::connect(process, &QProcess::readyReadStandardOutput,
		this, &Runner::readyRead);

	QProcess::connect(process, &QProcess::readyReadStandardError,
		this, &Runner::readyError);

	lib::log::debug("starting: {} {}", path.toStdString(),
		arguments.join(' ').toStdString());

	process->start(path, arguments);
	return {};
}

auto SpotifyClient::Runner::waitForStarted() const -> bool
{
	if (process == nullptr)
	{
		return false;
	}

	constexpr int timeout = 3 * 1000;
	return process->waitForStarted(timeout);
}

auto SpotifyClient::Runner::availableBackends() -> QStringList
{
	return SpotifyClient::Helper::availableBackends(path);
}

auto SpotifyClient::Runner::isRunning() const -> bool
{
	return process == nullptr
		? SpotifyClient::Helper::running(path)
		: process->isOpen();
}

void SpotifyClient::Runner::logOutput(const QByteArray &output)
{
	for (auto &line: QString(output).split('\n'))
	{
		if (line.isEmpty())
		{
			continue;
		}
		log << QPair<QDateTime, QString>(QDateTime::currentDateTime(), line);
	}
}

void SpotifyClient::Runner::readyRead() const
{
	logOutput(process->readAllStandardOutput());
}

void SpotifyClient::Runner::readyError() const
{
	logOutput(process->readAllStandardError());
}

auto SpotifyClient::Runner::getLog() -> const QList<QPair<QDateTime, QString>> &
{
	return log;
}
