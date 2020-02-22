#include "clienthandler.hpp"

using namespace spt;

ClientHandler::ClientHandler(QWidget *parent) : parent(parent)
{
}

ClientHandler::~ClientHandler()
{
	if (process == nullptr)
		return;
	process->close();
	delete process;
	process = nullptr;
}

QString ClientHandler::start()
{
	Settings settings;
	// Check if empty
	auto path = settings.sptPath();
	if (path.isEmpty())
		return "path is empty";
	// Check if path exists
	QFileInfo info(path);
	if (!info.exists())
		return "file in path does not exist";
	// Check if username exists
	auto username = settings.sptUser();
	if (username.isEmpty())
		return "no username provided";
	// Ask for password
	auto password = QInputDialog::getText(parent,
		"Enter password",
		QString("Enter password for Spotify user \"%1\":").arg(username));
	if (password.isEmpty())
		return "no password provided";
	// Attempt to start spotifyd
	process = new QProcess();
	process->start(path, {
		"--bitrate", QString::number(settings.sptBitrate()),
		"--device-name", "spotify-qt",
		"--username", username,
		"--password", password
	});
	return QString();
}