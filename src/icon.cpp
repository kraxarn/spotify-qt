#include "icon.hpp"

QString Icon::theme = QString();

QVariant Icon::useFallbackIcons = QVariant();

Icon::Icon()
{
}

QIcon Icon::get(const QString &name)
{
	if (theme.isNull())
		theme = MainWindow::hasDarkBackground() ? "dark" : "light";
	if (!useFallbackIcons.isValid())
		useFallbackIcons = Settings().useFallbackIcons();

	if (name.startsWith("logo:"))
		return QIcon(QString(":/res/logo/%1.svg")
			 .arg(name.right(name.length() - QString("logo:").length())));
	auto fallback = QString(":/res/ic/%1/%2.svg").arg(theme).arg(name);
	return useFallbackIcons.toBool()
		? QIcon(fallback) : QIcon::fromTheme(name, QIcon(fallback));
}
