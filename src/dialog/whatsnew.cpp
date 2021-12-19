#include "dialog/whatsnew.hpp"
#include "mainwindow.hpp"

Dialog::WhatsNew::WhatsNew(lib::settings &settings,
	const lib::http_client &httpClient, QWidget *parent)
	: QDialog(parent),
	settings(settings),
	httpClient(httpClient)
{
	auto *layout = new QVBoxLayout();
	auto *title = new QLabel(QString("%1 was updated to version %2")
		.arg(APP_NAME, APP_VERSION));

	auto titleFont = title->font();
	constexpr float titleFontMulti = 1.5F;
	auto fontSize = static_cast<float>(titleFont.pointSize()) * titleFontMulti;
	titleFont.setPointSize(static_cast<int>(fontSize));

	title->setFont(titleFont);
	layout->addWidget(title);
	text = new QTextEdit();
	text->setReadOnly(true);
	text->setPlainText("Loading changes...");

	layout->addWidget(text, 1);
	auto *buttons = new QDialogButtonBox(this);

	auto *dontShowAgain = buttons->addButton("Don't show again",
		QDialogButtonBox::RejectRole);
	QPushButton::connect(dontShowAgain, &QPushButton::clicked,
		this, &Dialog::WhatsNew::onDontShowAgain);

	auto *ok = buttons->addButton(QDialogButtonBox::Ok);
	QPushButton::connect(ok, &QPushButton::clicked,
		this, &Dialog::WhatsNew::onOk);

	layout->addWidget(buttons);
	setLayout(layout);
	resize(500, 400);
	setWindowTitle(QString("%1 %2").arg(APP_NAME, APP_VERSION));
}

void Dialog::WhatsNew::onDontShowAgain(bool /*checked*/)
{
	settings.general.show_changelog = false;
	settings.save();

	reject();
}

void Dialog::WhatsNew::onOk(bool /*checked*/)
{
	accept();
}

void Dialog::WhatsNew::showEvent(QShowEvent *event)
{
	lib::gh::api(httpClient).release(ORG_NAME, APP_NAME, APP_VERSION,
		[this](const lib::gh::release &release)
		{
			onReleaseInfo(release);
		});

	QDialog::showEvent(event);
}

void Dialog::WhatsNew::onReleaseInfo(const lib::gh::release &release)
{
	auto body = QString::fromStdString(release.body);
	if (body.isEmpty())
	{
		text->setText(QStringLiteral("No changes for this release"));
		return;
	}

	// Markdown formatting only supports Qt 5.14
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
	text->setMarkdown(body);
#else
	text->setText(body);
#endif
}
