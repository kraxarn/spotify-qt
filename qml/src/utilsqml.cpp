#include "utilsqml.hpp"

UtilsQml::UtilsQml(QObject *parent)
	: QObject(parent)
{
}

QString UtilsQml::formatTime(int ms)
{
	return Utils::formatTime(ms);
}