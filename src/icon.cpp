#include "icon.hpp"

QString Icon::theme = QString();

bool Icon::useFallbackIcons = false;

Icon::Icon()
{
}

QIcon Icon::get(const QString &name)
{
	if (theme.isNull())
		theme = Utils::darkBackground ? "dark" : "light";

	if (name.startsWith("logo:"))
		return QIcon(QString(":/res/logo/%1.svg")
			 .arg(name.right(name.length() - QString("logo:").length())));
	auto fallback = QString(":/res/ic/%1/%2.svg").arg(theme).arg(name);
	return useFallbackIcons
		? QIcon(fallback) : QIcon::fromTheme(name, QIcon(fallback));
}
