#include "lib/systeminfo.hpp"

lib::qt::system_info::system_info()
{
	// Qt version
	add(QStringLiteral("Qt version"), QStringLiteral(QT_VERSION_STR));

	// Desktop environment
	if (lib::system::has_env("XDG_CURRENT_DESKTOP"))
	{
		add(QStringLiteral("Current desktop"),
			QString::fromStdString(lib::system::env("XDG_CURRENT_DESKTOP")));
	}

	// Kernel
	add(QStringLiteral("Kernel"), QStringLiteral("%1 %2")
		.arg(QSysInfo::kernelType(), QSysInfo::kernelVersion()));

	// Product
	add(QStringLiteral("Product"), QSysInfo::prettyProductName());

	// Build ABI
	add(QStringLiteral("ABI"), QSysInfo::buildAbi());
}

void lib::qt::system_info::add(const QString &key, const QString &value)
{
	info[key] = value;
}

auto lib::qt::system_info::to_text() -> QString
{
	QString systemInfo;
	QMapIterator<QString, QString> iter(info);

	auto max = 0;
	while (iter.hasNext())
	{
		iter.next();
		const auto keySize = static_cast<int>(iter.key().length());
		if (keySize > max)
		{
			max = keySize;
		}
	}

	iter.toFront();
	while (iter.hasNext())
	{
		iter.next();
		systemInfo += QStringLiteral("%1: %2\n")
			.arg(iter.key(), -max)
			.arg(iter.value());
	}
	return systemInfo;
}

auto lib::qt::system_info::to_html() -> QString
{
	auto systemInfo = QStringLiteral("<table>");
	QMapIterator<QString, QString> i(info);
	while (i.hasNext())
	{
		i.next();
		systemInfo += QStringLiteral("<tr><td>%1:</td> <td>%2</td></tr>")
			.arg(i.key(), i.value());
	}
	return QStringLiteral("%1</table>").arg(systemInfo);
}
