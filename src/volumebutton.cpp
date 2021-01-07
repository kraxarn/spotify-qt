#include "volumebutton.hpp"

VolumeButton::VolumeButton(lib::settings &settings, spt::Spotify &spotify, QWidget *parent)
	: settings(settings),
	spotify(spotify),
	volume(new QSlider(this)),
	QToolButton(parent)
{
	// Volume slider
	volume->setOrientation(Qt::Orientation::Vertical);
	volume->setFixedHeight(100);
	volume->setMinimum(0);
	volume->setMaximum(20);
	volume->setValue(settings.general.last_volume > 0
		? settings.general.last_volume
		: spt::ClientHandler::getVolume() * 20);

	// Layout for volume slider
	auto volumeMenu = new QMenu(this);
	volumeMenu->setContentsMargins(2, 6, 2, 6);
	auto volumeLayout = new QVBoxLayout();
	volumeLayout->addWidget(volume);
	volumeMenu->setLayout(volumeLayout);

	setText("Volume");
	updateIcon();
	setPopupMode(QToolButton::InstantPopup);
	setMenu(volumeMenu);

	QAbstractSlider::connect(volume, &QAbstractSlider::valueChanged, [this](int value)
	{
		updateIcon();
	});

	if (settings.general.pulse_volume)
	{
		// If using PulseAudio for volume control, update on every tick
		QSlider::connect(volume, &QAbstractSlider::valueChanged, [](int value)
		{
			spt::ClientHandler::setVolume((float)value * 0.05f);
		});
	}
	else
	{
		// If using Spotify for volume control, only update on release
		QSlider::connect(volume, &QAbstractSlider::sliderReleased, [this]()
		{
			auto status = this->spotify.setVolume(volume->value() * 5);
			if (!status.isEmpty())
			{
				auto window = MainWindow::find(parentWidget());
				if (window != nullptr)
					window->setStatus(QString("Failed to set volume: %1").arg(status), true);
			}
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
	auto vol = volume->value() * 5;
	setIcon(Icon::get(QString("audio-volume-%1")
		.arg(vol < 33 ? "low" : vol > 66 ? "high" : "medium")));
}

void VolumeButton::setVolume(int value)
{
	volume->setValue(value);
}
