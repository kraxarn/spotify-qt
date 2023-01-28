#include "widget/about.hpp"
#include "mainwindow.hpp"

About::About(QWidget *parent)
	: QWidget(parent)
{
	auto *layout = new QVBoxLayout(this);
	layout->setAlignment(Qt::AlignCenter);

	// Logo
	auto *titleLogo = new QLabel();
	titleLogo->setPixmap(Icon::get(QString("logo:%1")
		.arg(APP_ICON))
		.pixmap(logoSize, logoSize));
	layout->addWidget(titleLogo, 0, Qt::AlignHCenter);

	// Title
	auto *titleAppName = new QLabel(QString("%1 %2")
		.arg(APP_NAME, APP_VERSION));
	auto appNameFont = titleAppName->font();
	appNameFont.setPointSize((int) (appNameFont.pointSizeF() * appNameFontMulti));
	titleAppName->setFont(appNameFont);
	layout->addWidget(titleAppName, 0, Qt::AlignHCenter);

	// Qt version
	auto *qtVersion = new QLabel(QString("Built using Qt %1").arg(QT_VERSION_STR), this);
	layout->addWidget(qtVersion, 0, Qt::AlignHCenter);

	// Build info
#ifdef GIT_COMMIT
	auto *commit = new QLabel(QString("Commit %1").arg(GIT_COMMIT), this);
	layout->addWidget(commit, 0, Qt::AlignHCenter);
#endif

	// User info
	auto *mainWindow = MainWindow::find(parentWidget());
	if (mainWindow != nullptr)
	{
		layout->addSpacing(16);
		layout->addWidget(new QLabel(QString("Hello %1!")
				.arg(QString::fromStdString(mainWindow->getCurrentUser().display_name))),
			0, Qt::AlignHCenter);
	}
}
