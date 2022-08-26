#include "dialog/auth.hpp"
#include <QMessageBox>

Dialog::Auth::Auth(lib::settings &settings, QWidget *parent)
	: Base(parent),
	settings(settings)
{
	setTitle(QStringLiteral("Authentication failed"));
	auto *layout = Base::layout<QVBoxLayout>();
	layout->setContentsMargins(margin, margin, margin, margin);
	layout->setSpacing(margin / 2);

	errorInfo = new QLabel(this);
	errorInfo->setText(QStringLiteral("Token expired."));
	layout->addWidget(errorInfo);

	auto *description = new QLabel(this);
	description->setText(QStringLiteral("Please reauthenticate."));
	layout->addWidget(description);

	reauthenticate = addButton(QStringLiteral("Reauthenticate"), QDialogButtonBox::AcceptRole);

	QPushButton::connect(reauthenticate, &QPushButton::clicked,
		this, &Dialog::Auth::onReauthenticate);

	auth = new spt::AuthServer(settings, this);

	spt::AuthServer::connect(auth, &spt::AuthServer::success,
		this, &Dialog::Auth::onAuthSuccess);

	spt::AuthServer::connect(auth, &spt::AuthServer::failed,
		this, &Dialog::Auth::onAuthFailed);
}

void Dialog::Auth::show(const lib::spt::error &error)
{
	errorInfo->setText(QString(error.what()));
}

void Dialog::Auth::onReauthenticate(bool /*checked*/)
{
	reauthenticate->setEnabled(false);

	if (auth->isListening() || auth->listen())
	{
		auth->openUrl(nullptr);
	}
}

void Dialog::Auth::onAuthSuccess()
{
	emit success();
	close();
}

void Dialog::Auth::onAuthFailed(const QString &message)
{
	QMessageBox::warning(nullptr, windowTitle(), message);
	emit failed(message);
	reauthenticate->setEnabled(true);
}
