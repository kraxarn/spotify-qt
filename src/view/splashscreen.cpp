#include "view/splashscreen.hpp"
#include "spotify/authserver.hpp"

#define TITLE QStringLiteral("Connection failed")

SplashScreen::SplashScreen(lib::settings &settings, spt::Spotify &spotify, QWidget *parent)
	: parent(parent),
	settings(settings),
	spotify(spotify)
{
	QPixmap background(backgroundSize, backgroundSize);
	background.fill(QColor::fromRgb(backgroundColor));

	QImage image(background.size(), QImage::Format_ARGB32);
	QPainter painter(&image);
	painter.drawPixmap(0, 0, Image::mask(background, MaskShape::App));

	painter.drawPixmap(logoPos, logoPos, Icon::get(QString("logo:%1")
		.arg(APP_ICON))
		.pixmap(logoSize, logoSize));

	setPixmap(QPixmap::fromImage(image));
	showMessage(QStringLiteral("Please wait..."));
}

void SplashScreen::showEvent(QShowEvent *event)
{
	QWidget::showEvent(event);
	showMessage(QStringLiteral("Connecting..."));

	try
	{
		spotify.refresh();
		close(true);
	}
	catch (const nlohmann::json::exception &e)
	{
		QMessageBox::warning(this, TITLE,
			QString("Failed to parse response from Spotify:\n%1").arg(e.what()));

		close(false);
	}
	catch (const lib::spt::error &e)
	{
		QMessageBox::warning(this, TITLE,
			QString("%1.\nPlease reauthenticate.").arg(e.what()));

		spt::AuthServer auth(settings, this);
		if (auth.listen())
		{
			auth.openUrl(this);
		}

		spt::AuthServer::connect(&auth, &spt::AuthServer::success, this, [this]()
		{
			close(true);
		});

		spt::AuthServer::connect(&auth, &spt::AuthServer::failed, [this](const QString &message)
		{
			QMessageBox::warning(this, TITLE, message);
			close(false);
		});
	}
	catch (const std::exception &e)
	{
		QMessageBox::warning(this, TITLE,
			QString("Failed to connect to Spotify, check your connection and try again:\n%1")
				.arg(e.what()));

		close(false);
	}
}

void SplashScreen::showMessage(const QString &message)
{
	QSplashScreen::showMessage(message,
		Qt::AlignBottom, QColor::fromRgb(foregroundColor));
}

void SplashScreen::close(bool successful)
{
	// Widgets can't be closed directly after being opened,
	// therefore waiting is required

	if (successful)
	{
		showMessage(QStringLiteral("Welcome!"));
	}

	QTimer::singleShot(closeTimer, [this, successful]()
	{
		if (successful)
		{
			emit success();
			finish(parent);
		}
		else
		{
			emit failed();
			QSplashScreen::close();
		}
	});
}
