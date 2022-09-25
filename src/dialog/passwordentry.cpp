#include "dialog/passwordentry.hpp"

#ifdef USE_KEYCHAIN
#include "util/keychain.hpp"
#endif

Dialog::PasswordEntry::PasswordEntry(SpotifyClient::Runner *runner, QWidget *parent)
	: Base(parent),
	runner(runner)
{
	setTitle(QStringLiteral("Password"));

	auto *layout = Base::layout<QVBoxLayout>();

	container = new QGroupBox(this);
	container->setTitle(QStringLiteral("Password for Spotify user"));
	auto *containerLayout = new QVBoxLayout(container);

	password = new QLineEdit(this);
	password->setMinimumWidth(300);
	password->setEchoMode(QLineEdit::Password);
	containerLayout->addWidget(password);
	layout->addWidget(container);

#ifdef USE_KEYCHAIN
	rememberMe = new QCheckBox(this);
	rememberMe->setText(QStringLiteral("Remember me"));
	rememberMe->setChecked(true);
	layout->addWidget(rememberMe);
#endif

	Base::addAction(DialogAction::Ok);
	Base::addAction(DialogAction::Cancel);
}

void Dialog::PasswordEntry::show(const QString &username)
{
	currentUsername = username;
	container->setTitle(QString("Password for Spotify user \"%1\"").arg(username));
	QWidget::show();
}

void Dialog::PasswordEntry::onOk(bool checked)
{
	const auto passwordText = password->text();

#ifdef USE_KEYCHAIN
	if (rememberMe->isChecked() && !currentUsername.isEmpty())
	{
		Keychain::setPassword(currentUsername, passwordText);
	}
#endif

	runner->start(currentUsername, passwordText);
	Base::onOk(checked);
}

void Dialog::PasswordEntry::onCancel(bool checked)
{
	// Start anyway with empty password to emit statusChanged signal
	runner->start(currentUsername, {});

	Base::onCancel(checked);
}
