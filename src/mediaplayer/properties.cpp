#include "properties.hpp"

using namespace mp;

Properties::Properties(QObject *parent) : QDBusAbstractAdaptor(parent)
{
}

void Properties::updateMetadata(const QMap<QString, QVariant> &metadata)
{
	emit PropertiesChanged("org.mpris.MediaPlayer2.Player", metadata, {});
}