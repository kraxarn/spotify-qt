#include "whatsnewdialog.hpp"

WhatsNewDialog::WhatsNewDialog(const QString &tag, lib::settings &settings, QWidget *parent)
	: settings(settings),
	QDialog(parent)
{
	auto *window = MainWindow::find(parentWidget());
	if (window == nullptr)
	{
		return;
	}

	auto json = window->getJson(
		QString("https://api.github.com/repos/kraxarn/spotify-qt/releases/tags/%1")
			.arg(tag)).object();
	auto body = json["body"].toString();
	if (body.isEmpty())
	{
		return;
	}

	auto *layout = new QVBoxLayout();
	auto *title = new QLabel(QString("spotify-qt was updated to version %1").arg(tag));

	auto titleFont = title->font();
	constexpr float titleFontMulti = 1.5F;
	auto fontSize = static_cast<float>(titleFont.pointSize()) * titleFontMulti;
	titleFont.setPointSize(static_cast<int>(fontSize));

	title->setFont(titleFont);
	layout->addWidget(title);
	auto *text = new QTextEdit();
	text->setReadOnly(true);

	// Markdown formatting only supports Qt 5.14
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
	text->setMarkdown(body);
#else
	text->setText(body);
#endif

	layout->addWidget(text, 1);
	auto *buttons = new QDialogButtonBox(this);

	auto *dontShowAgain = buttons->addButton("Don't show again",
		QDialogButtonBox::RejectRole);
	QPushButton::connect(dontShowAgain, &QPushButton::clicked,
		this, &WhatsNewDialog::onDontShowAgain);

	auto *ok = buttons->addButton(QDialogButtonBox::Ok);
	QPushButton::connect(ok, &QPushButton::clicked,
		this, &WhatsNewDialog::onOk);

	layout->addWidget(buttons);
	setLayout(layout);
	resize(500, 400);
	setWindowTitle(QString("spotify-qt %1").arg(tag));

	success = true;
}

auto WhatsNewDialog::isValid() const -> bool
{
	return success;
}

void WhatsNewDialog::onDontShowAgain(bool /*checked*/)
{
	settings.general.show_changelog = false;
	settings.save();

	reject();
}

void WhatsNewDialog::onOk(bool /*checked*/)
{
	accept();
}
