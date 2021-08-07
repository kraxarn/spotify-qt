#pragma once

#include "lib/spotify/api.hpp"

#include "client/clienthelper.hpp"
#include "util/icon.hpp"

#include <QMenu>
#include <QSlider>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWheelEvent>

class VolumeButton: public QToolButton
{
Q_OBJECT

public:
	VolumeButton(lib::settings &settings, lib::spt::api &spotify, QWidget *parent);
	~VolumeButton();

	void updateIcon();
	void setVolume(int value);

protected:
	void wheelEvent(QWheelEvent *event) override;

private:
	QSlider *volume;
	lib::settings &settings;
	lib::spt::api &spotify;
};
