#include "common/systeminfo.hpp"

SystemInfo::SystemInfo()
{
	// Qt version
	add("Qt version", QT_VERSION_STR);

	// Desktop environment
	if (SystemUtils::hasEnv("XDG_CURRENT_DESKTOP"))
	{
		add("Current desktop", SystemUtils::env("XDG_CURRENT_DESKTOP"));
	}

	// Kernel
	add("Kernel", QString("%1 %2")
		.arg(QSysInfo::kernelType(), QSysInfo::kernelVersion()));

	// Product
	add("Product", QSysInfo::prettyProductName());

	// Build ABI
	add("ABI", QSysInfo::buildAbi());
}

void SystemInfo::add(const QString &key, const QString &value)
{
	info[key] = value;
}

auto SystemInfo::toText() -> QString
{
	QString systemInfo;
	QMapIterator<QString, QString> i(info);
	while (i.hasNext())
	{
		i.next();
		systemInfo += QString("%1: %2\n")
			.arg(i.key(), i.value());
	}
	return systemInfo;
}

auto SystemInfo::toHtml() -> QString
{
	QString systemInfo("<table>");
	QMapIterator<QString, QString> i(info);
	while (i.hasNext())
	{
		i.next();
		systemInfo += QString("<tr><td>%1:</td> <td>%2</td></tr>")
			.arg(i.key(), i.value());
	}
	return QString("%1</table>").arg(systemInfo);
}
