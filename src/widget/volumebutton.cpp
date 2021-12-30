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
	volume->setValue(settings.general.last_volume);

	// Layout for volume slider
	auto *volumeMenu = new QMenu(this);
	volumeMenu->setContentsMargins(vMargin, hMargin, vMargin, hMargin);
	auto *volumeLayout = new QVBoxLayout();
	volumeLayout->addWidget(volume);
	volumeMenu->setLayout(volumeLayout);

	setText("Volume");
	updateIcon();
	setPopupMode(QToolButton::InstantPopup);
	setMenu(volumeMenu);

	QAbstractSlider::connect(volume, &QAbstractSlider::valueChanged, [this](int /*value*/)
	{
		updateIcon();
	});

	// Update volume on release
	QSlider::connect(volume, &QAbstractSlider::sliderReleased, [this]()
	{
		this->spotify.set_volume(volume->value() * step,
			[](const std::string &status)
			{
				if (status.empty())
				{
					return;
				}

				StatusMessage::error(QString("Failed to set volume: %1")
					.arg(QString::fromStdString(status)));
			});
	});
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

void VolumeButton::updateIcon()
{
	auto vol = volume->value();
	setIcon(Icon::get(QString("audio-volume-%1")
		.arg(vol < lowVolume
			? "low"
			: vol > highVolume
				? "high"
				: "medium")));
}

void VolumeButton::setVolume(int value)
{
	volume->setValue(value);
}
