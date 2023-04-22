#include "widget/about.hpp"
#include "mainwindow.hpp"

About::About(QWidget *parent)
	: QWidget(parent)
{
	auto *layout = new QVBoxLayout(this);
	layout->setAlignment(Qt::AlignCenter);

	auto *infoLayout = new QHBoxLayout();
	infoLayout->addWidget(appLogo(), 0, Qt::AlignHCenter);
	infoLayout->addWidget(appInfo(), 0, Qt::AlignHCenter);
	layout->addLayout(infoLayout);

	// User info
	auto *mainWindow = MainWindow::find(parentWidget());
	if (mainWindow != nullptr)
	{
		layout->addWidget(new QLabel(QString("Hello %1!")
				.arg(QString::fromStdString(mainWindow->getCurrentUser().display_name))),
			0, Qt::AlignHCenter);
	}
}

auto About::appLogo() -> QWidget *
{
	auto *titleLogo = new QLabel(this);

	const auto icon = Icon::get(QString("logo:%1").arg(APP_ICON));
	const auto pixmap = icon.pixmap(logoSize, logoSize);
	titleLogo->setPixmap(pixmap);

	return titleLogo;
}

auto About::appInfo() -> QWidget *
{
	auto *content = new QWidget(this);
	auto *layout = new QVBoxLayout(content);
	layout->setSpacing(4);
	layout->addStretch(1);

	// Title
	auto *titleAppName = new QLabel(QString("%1 %2")
		.arg(APP_NAME, APP_VERSION));
	auto appNameFont = titleAppName->font();
	appNameFont.setPointSize((int) (appNameFont.pointSizeF() * appNameFontMulti));
	titleAppName->setFont(appNameFont);
	layout->addWidget(titleAppName);

	// Qt version
	auto *qtVersion = new QLabel(QString("Built using Qt %1").arg(QT_VERSION_STR), this);
	layout->addWidget(qtVersion);

	// Build info
#ifdef GIT_COMMIT
	auto *commit = new QLabel(QString("Commit %1").arg(GIT_COMMIT), this);
	layout->addWidget(commit);

	auto *buildDate = new QLabel(QString("Built on %1").arg(__DATE__), this);
	layout->addWidget(buildDate);
#endif

	layout->addStretch(1);
	return content;
}
