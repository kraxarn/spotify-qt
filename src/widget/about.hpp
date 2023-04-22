#pragma once

#include <QWidget>

class About: public QWidget
{
public:
	explicit About(QWidget *parent);

private:
	static constexpr int logoSize = 96;
	static constexpr float appNameFontMulti = 1.5F;

	auto appLogo() -> QWidget *;
	auto appInfo() -> QWidget *;
};
