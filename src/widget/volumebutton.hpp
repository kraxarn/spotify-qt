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
	~VolumeButton() override;

	void updateIcon();
	void setVolume(int value);

protected:
	void wheelEvent(QWheelEvent *event) override;

private:
	/** Height of volume slider widget */
	static constexpr int height = 100;

	/** Minimum volume */
	static constexpr int minimum = 0;
	/** Maximum volume */
	static constexpr int maximum = 20;
	/** How much each step adjusts volume */
	static constexpr int step = 100 / maximum;

	/** Vertical margins */
	static constexpr int vMargin = 2;
	/** Horizontal margins */
	static constexpr int hMargin = 6;

	/** Threshold for low volume icon */
	static constexpr int lowVolume = step / 3;
	/** Threshold for high volume icon */
	static constexpr int highVolume = step / 3 * 2;

	QSlider *volume;
	lib::settings &settings;
	lib::spt::api &spotify;
};
