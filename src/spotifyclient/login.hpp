#pragma once

#include <QObject>
#include <QProcess>
#include <QWidget>

namespace SpotifyClient
{
	class Login final: public QObject
	{
	Q_OBJECT

	public:
		explicit Login(QWidget *parent);

		void run(const QString &clientPath, const QString &cachePath) const;

	signals:
		void loginSuccess();
		void loginFailed(const QString &message);

	private:
		QProcess *process;

		void onOutput(const QString &output);
		void onReadyReadOutput();
		void onReadyReadError();
		void onErrorOccurred(QProcess::ProcessError error);
	};
}
