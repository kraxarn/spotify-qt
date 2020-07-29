#include "icon.hpp"

bool Icon::useFallbackIcons = false;

QIcon Icon::get(const QString &name)
{
	if (name.startsWith("logo:"))
		return QIcon(QString(":/res/logo/%1.svg")
			 .arg(name.right(name.length() - QString("logo:").length())));
	auto fallback = QString(":/res/ic/%1/%2.svg")
		.arg(Utils::darkBackground ? "dark" : "light")
		.arg(name);
	return useFallbackIcons
		? QIcon(fallback) : QIcon::fromTheme(name, QIcon(fallback));
}
