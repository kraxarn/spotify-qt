#include "systeminfodialog.hpp"

SystemInfoDialog::SystemInfoDialog(QWidget *parent) : QDialog(parent)
{
	auto layout = new QVBoxLayout();
	setLayout(layout);
	setWindowTitle("System info");

	auto textInfo = new QTextEdit(this);
	textInfo->setHtml(systemInfo());
	textInfo->setReadOnly(true);
	layout->addWidget(textInfo);

	auto infoAbout = new QLabel(
		"This information could be useful when reporting bugs. "
			"Additional information, depending on the type of issue, may be more helpful.", this);
	infoAbout->setWordWrap(true);
	layout->addWidget(infoAbout);

	auto buttons = new QDialogButtonBox();
	QPushButton::connect(buttons->addButton(QDialogButtonBox::Ok),
		&QPushButton::clicked, [this](bool checked) {
		accept();
	});
	layout->addWidget(buttons);
}

QString SystemInfoDialog::systemInfo()
{
	QMap<QString, QString> info;

	// OS release
	QFile lsbFile("/etc/lsb-release");
	if (lsbFile.exists() && lsbFile.open(QIODevice::ReadOnly))
		for (const auto &line : QString(lsbFile.readAll()).split('\n'))
			if (line.startsWith("DISTRIB_DESCRIPTION"))
				info["LSB release"] = line.right(line.length() - 21).left(line.length() - 22);

	// Linux version
	QFile procFile("/proc/version");
	if (procFile.exists() && procFile.open(QIODevice::ReadOnly))
		info["Linux version"] = QString(procFile.readAll()).split(' ')[2];

	// Qt version
	info["Qt version"] = QT_VERSION_STR;

	// spotify-qt version
	info["App version"] = APP_VERSION;

	// Desktop environment
	if (qEnvironmentVariableIsSet("XDG_CURRENT_DESKTOP"))
		info["Current desktop"] = qEnvironmentVariable("XDG_CURRENT_DESKTOP");

	QString systemInfo("<table>");
	QMapIterator<QString, QString> i(info);
	while (i.hasNext())
	{
		i.next();
		systemInfo += QString("<tr><td>%1:</td> <td>%2</td></tr>").arg(i.key()).arg(i.value());
	}
	return systemInfo + "</table>";
}