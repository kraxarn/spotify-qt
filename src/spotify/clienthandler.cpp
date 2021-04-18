#include "../mainwindow.hpp"
#include "clienthandler.hpp"

using namespace spt;

ClientHandler::ClientHandler(const lib::settings &settings, const lib::paths &paths,
	QWidget *parent)
	: settings(settings),
	lib::spt::client_handler(settings, paths),
	QObject(parent)
{
	process = new QProcess(parent);
}

ClientHandler::~ClientHandler()
{
	if (process != nullptr)
	{
		process->close();
	}
}

auto ClientHandler::get_keyring_password(const std::string &username) -> std::string
{
#ifdef USE_DBUS
	KWallet keyring(QString::fromStdString(username));
	if (settings.spotify.keyring_password && keyring.unlock())
	{
		return keyring.readPassword().toStdString();
	}
#endif
	return std::string();
}

void ClientHandler::set_keyring_password(const std::string &username,
	const std::string &password)
{
#ifdef USE_DBUS
	if (settings.spotify.keyring_password)
	{
		KWallet keyring(QString::fromStdString(username));
		keyring.writePassword(QString::fromStdString(password));
	}
#endif
}

auto ClientHandler::get_password(const std::string &username) -> std::string
{
	auto *parentWidget = dynamic_cast<QWidget *>(parent());

	return QInputDialog::getText(parentWidget,
		"Enter password",
		QString("Enter password for Spotify user \"%1\":")
			.arg(QString::fromStdString(username)),
		QLineEdit::Password).toStdString();
}

void ClientHandler::start_process(const ghc::filesystem::path &path,
	const std::vector<std::string> &arguments)
{
	QProcess::connect(process, &QProcess::readyReadStandardOutput,
		this, &ClientHandler::readyRead);
	QProcess::connect(process, &QProcess::readyReadStandardError,
		this, &ClientHandler::readyError);

	process->start(QString::fromStdString(path.string()),
		QtUtils::toStringList(arguments));
}

auto ClientHandler::start_process_and_wait(const ghc::filesystem::path &path,
	const std::vector<std::string> &arguments) -> std::string
{
	QProcess tempProcess;

	tempProcess.start(QString::fromStdString(path),
		QtUtils::toStringList(arguments));
	tempProcess.waitForFinished();

	return tempProcess.readAllStandardOutput()
		.trimmed().toStdString();
}

void ClientHandler::readyRead()
{
	log_output(process->readAllStandardOutput().toStdString());
}

void ClientHandler::readyError()
{
	log_output(process->readAllStandardError().toStdString());
}
