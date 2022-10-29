#include "lib/qt/systeminfo.hpp"

lib::qt::system_info::system_info()
{
	// Qt version
	add("Qt version", QT_VERSION_STR);

	// Desktop environment
	if (lib::system::has_env("XDG_CURRENT_DESKTOP"))
	{
		add("Current desktop",
			QString::fromStdString(lib::system::env("XDG_CURRENT_DESKTOP")));
	}

	// Kernel
	add("Kernel", QString("%1 %2")
		.arg(QSysInfo::kernelType(), QSysInfo::kernelVersion()));

	// Product
	add("Product", QSysInfo::prettyProductName());

	// Build ABI
	add("ABI", QSysInfo::buildAbi());

#ifdef USE_KEYCHAIN
	add(QStringLiteral("Keychain support"), QStringLiteral("Yes"));
#else
	add(QStringLiteral("Keychain support"), QStringLiteral("No"));
#endif
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
		systemInfo += QString("%1: %2\n")
			.arg(iter.key(), -max)
			.arg(iter.value());
	}
	return systemInfo;
}

auto lib::qt::system_info::to_html() -> QString
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
