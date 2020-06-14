#pragma once

class VolumeButton;

#include "icon.hpp"
#include "spotify/clienthandler.hpp"

#include <QMenu>
#include <QToolButton>
#include <QWheelEvent>
#include <QDebug>
#include <QVBoxLayout>
#include <QSlider>

class VolumeButton : public QToolButton
{
	Q_OBJECT

public:
	VolumeButton(Settings &settings, spt::Spotify &spotify, QWidget *parent);
	~VolumeButton();

	void updateIcon();

protected:
	void wheelEvent(QWheelEvent *event) override;


private:
	QSlider *volume;
	Settings &settings;
	spt::Spotify &spotify;
};