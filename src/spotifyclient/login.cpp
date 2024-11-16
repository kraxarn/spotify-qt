#include "spotifyclient/login.hpp"

#include "spotifyclient/helper.hpp"
#include "util/url.hpp"

#include <QObject>
#include <QProcess>

SpotifyClient::Login::Login(QWidget *parent)
	: QObject(parent),
	process(new QProcess(this))
{
	connect(process, &QProcess::readyReadStandardOutput,
		this, &Login::onReadyReadOutput);

	connect(process, &QProcess::readyReadStandardError,
		this, &Login::onReadyReadError);

	connect(process, &QProcess::errorOccurred,
		this, &Login::onErrorOccurred);
}

void SpotifyClient::Login::run(const QString &clientPath, const QString &cachePath) const
{
	const QStringList arguments({
		"--enable-oauth",
		"--cache", cachePath,
	});

	process->start(clientPath, arguments);
}

void SpotifyClient::Login::onReadyReadOutput()
{
	const auto output = process->readAllStandardOutput();

	for (auto &line: QString(output).split('\n'))
	{
		if (line.isEmpty())
		{
			continue;
		}

		const auto urlIndex = line.indexOf(QStringLiteral("https://accounts.spotify.com/authorize"));
		if (urlIndex >= 0)
		{
			const auto url = line.right(line.length() - urlIndex - 1);
			auto *parent = qobject_cast<QWidget *>(QObject::parent());
			Url::open(url, LinkType::Web, parent);
			continue;
		}

		if (line.contains(QStringLiteral("Authenticated")))
		{
			emit loginSuccess();
			return;
		}
	}
}

void SpotifyClient::Login::onReadyReadError()
{
	const auto output = process->readAllStandardError();
	emit loginFailed(QString(output));
}

void SpotifyClient::Login::onErrorOccurred(const QProcess::ProcessError error)
{
	const auto message = Helper::processErrorToString(error);
	emit loginFailed(message);
}
