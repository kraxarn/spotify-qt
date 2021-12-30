#include "volumebutton.hpp"
#include "mainwindow.hpp"

VolumeButton::VolumeButton(lib::settings &settings, lib::spt::api &spotify, QWidget *parent)
	: QToolButton(parent),
	volume(new QSlider(this)),
	settings(settings),
	spotify(spotify)
{
	// Volume slider
	volume->setOrientation(Qt::Orientation::Vertical);
	volume->setFixedHeight(height);
	volume->setMinimum(minimum);
	volume->setMaximum(maximum);

	// Assume no volume is full volume
	auto volumeValue = settings.general.last_volume;
	if (volumeValue == minimum)
	{
		volumeValue = maximum;
	}
	volume->setValue(volumeValue);

	// Layout for volume slider
	auto *volumeMenu = new QMenu(this);
	volumeMenu->setContentsMargins(vMargin, hMargin, vMargin, hMargin);
	auto *volumeLayout = new QVBoxLayout();
	volumeLayout->addWidget(volume);
	volumeMenu->setLayout(volumeLayout);

	setText(QStringLiteral("Volume"));
	updateIcon(volumeValue);
	setPopupMode(QToolButton::InstantPopup);
	setMenu(volumeMenu);

	QAbstractSlider::connect(volume, &QAbstractSlider::valueChanged,
		this, &VolumeButton::onVolumeValueChanged);

	QSlider::connect(volume, &QAbstractSlider::sliderReleased,
		this, &VolumeButton::onVolumeSliderReleased);
}

VolumeButton::~VolumeButton()
{
	settings.general.last_volume = volume->value();
	settings.save();
}

void VolumeButton::wheelEvent(QWheelEvent *event)
{
	volume->setValue(volume->value() + (event->angleDelta().y() < 0 ? -1 : 1));
	event->accept();
}

void VolumeButton::updateIcon(int value)
{
	setIcon(Icon::get(QString("audio-volume-%1")
		.arg(getVolumeLevel(value))));

	setToolTip(getVolumeInfo(value));
}

auto VolumeButton::getVolumeLevel(int value) -> QString
{
	if (value < lowVolume)
	{
		return QStringLiteral("low");
	}

	if (value > highVolume)
	{
		return QStringLiteral("high");
	}

	return QStringLiteral("medium");
}

auto VolumeButton::getVolumeInfo(int value) -> QString
{
	return QString("%1 %")
		.arg(value * step);
}

void VolumeButton::setVolume(int value)
{
	volume->setValue(value);
}

void VolumeButton::onVolumeValueChanged(int value)
{
	updateIcon(value);
}

void VolumeButton::onVolumeSliderReleased()
{
	spotify.set_volume(volume->value() * step, [](const std::string &status)
	{
		if (!status.empty())
		{
			StatusMessage::error(QString("Failed to set volume: %1")
				.arg(QString::fromStdString(status)));
		}
	});
}
