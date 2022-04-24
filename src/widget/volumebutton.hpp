#pragma once

#include "lib/spotify/api.hpp"

#include "spotifyclient/runner.hpp"
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

	void setVolume(int value);

protected:
	void wheelEvent(QWheelEvent *event) override;

private:
	/** Height of volume slider widget */
	static constexpr int height = 100;
	/** Width of menu */
	static constexpr int width = 44;

	/** Minimum volume */
	static constexpr int minimum = 0;
	/** Maximum volume */
	static constexpr int maximum = 20;
	/** How much each step adjusts volume */
	static constexpr int step = 100 / maximum;

	/** Threshold for low volume icon */
	static constexpr int lowVolume = static_cast<int>(step * (1.0 / 3.0));
	/** Threshold for high volume icon */
	static constexpr int highVolume = static_cast<int>(step * (2.0 / 3.0));

	bool changing = false;
	QSlider *volume;
	QPushButton *volumeUp;
	QPushButton *volumeDown;

	lib::settings &settings;
	lib::spt::api &spotify;

	/**
	 * Update widgets on value change
	 */
	void update(int value);

	/**
	 * Icon for volume level, "audio-volume-low/medium/high"
	 */
	static auto getVolumeIcon(int value) -> QIcon;

	/**
	 * "{volume percentage} %"
	 */
	static auto getVolumeInfo(int value) -> QString;

	/**
	 * Send request to set volume
	 */
	void setSpotifyVolume(int value);

	/**
	 * Modify volume with set number of steps
	 */
	void changeVolume(int steps);

	void onVolumeValueChanged(int value);
	void onVolumeSliderPressed();
	void onVolumeSliderReleased();
	void onVolumeUp(bool checked);
	void onVolumeDown(bool checked);

	auto createButton(const QString &text) -> QPushButton *;
};
