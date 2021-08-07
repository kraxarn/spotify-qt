#include "volumebutton.hpp"
#include "mainwindow.hpp"

VolumeButton::VolumeButton(lib::settings &settings, lib::spt::api &spotify, QWidget *parent)
	: settings(settings),
	spotify(spotify),
	volume(new QSlider(this)),
	QToolButton(parent)
{
	// Volume slider
	volume->setOrientation(Qt::Orientation::Vertical);
	volume->setFixedHeight(height);
	volume->setMinimum(minimum);
	volume->setMaximum(maximum);
	volume->setValue(settings.general.last_volume > minimum
		? settings.general.last_volume
		: static_cast<int>(spt::ClientHelper::getVolume() * 20.F));

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

	if (settings.general.pulse_volume)
	{
		// If using PulseAudio for volume control, update on every tick
		QSlider::connect(volume, &QAbstractSlider::valueChanged, [](int value)
		{
			spt::ClientHelper::setVolume(static_cast<float>(value) * 0.05f);
		});
	}
	else
	{
		// If using Spotify for volume control, only update on release
		QSlider::connect(volume, &QAbstractSlider::sliderReleased, [this]()
		{
			this->spotify.set_volume(volume->value() * step,
				[this](const std::string &status)
				{
					if (!status.empty())
					{
						auto *window = MainWindow::find(parentWidget());
						if (window != nullptr)
						{
							window->status(lib::fmt::format("Failed to set volume: {}",
								status), true);
						}
					}
				});
		});
	}
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
