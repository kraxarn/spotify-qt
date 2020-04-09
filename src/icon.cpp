#include "icon.hpp"

QString Icon::theme = QString();

Icon::Icon()
{
}

QIcon Icon::get(const QString &name)
{
	if (theme.isNull())
		theme = Settings().darkTheme() ? "dark" : "light";
	if (name.startsWith("logo:"))
		return QIcon(QString(":/res/logo/%1.svg")
			 .arg(name.right(name.length() - QString("logo:").length())));
	return QIcon::fromTheme(name, QIcon(QString(":/res/ic/%1/%2.svg")
		.arg(theme).arg(name)));
}
