#include "volumebutton.hpp"
#include "mainwindow.hpp"

VolumeButton::VolumeButton(lib::settings &settings, lib::spt::api &spotify, QWidget *parent)
	: QToolButton(parent),
	settings(settings),
	spotify(spotify)
{
	// Volume slider
	volume = new QSlider(this);
	volume->setOrientation(Qt::Orientation::Vertical);
	volume->setFixedHeight(height);
	volume->setFixedWidth(width);
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

	volumeUp = createButton("+");
	QPushButton::connect(volumeUp, &QPushButton::clicked,
		this, &VolumeButton::onVolumeUp);

	volumeDown = createButton("-");
	QPushButton::connect(volumeDown, &QPushButton::clicked,
		this, &VolumeButton::onVolumeDown);

	auto *volumeLayout = new QVBoxLayout();
	volumeLayout->addWidget(volumeUp);
	volumeLayout->addWidget(volume);
	volumeLayout->addWidget(volumeDown);
	volumeMenu->setLayout(volumeLayout);

	setText(QStringLiteral("Volume"));
	update(volumeValue);
	setPopupMode(QToolButton::InstantPopup);
	setMenu(volumeMenu);

	QAbstractSlider::connect(volume, &QAbstractSlider::valueChanged,
		this, &VolumeButton::onVolumeValueChanged);

	QAbstractSlider::connect(volume, &QAbstractSlider::sliderPressed,
		this, &VolumeButton::onVolumeSliderPressed);

	QAbstractSlider::connect(volume, &QAbstractSlider::sliderReleased,
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

void VolumeButton::update(int value)
{
	setIcon(getVolumeIcon(value));
	setToolTip(getVolumeInfo(value));

	volumeUp->setEnabled(value < maximum);
	volumeDown->setEnabled(value > minimum);
}

auto VolumeButton::getVolumeIcon(int value) -> QIcon
{
	if (value < lowVolume)
	{
		return Icon::get(QStringLiteral("audio-volume-low"));
	}

	if (value > highVolume)
	{
		return Icon::get(QStringLiteral("audio-volume-high"));
	}

	return Icon::get(QStringLiteral("audio-volume-medium"));
}

auto VolumeButton::getVolumeInfo(int value) -> QString
{
	return QString("%1 %")
		.arg(value * step);
}

void VolumeButton::setVolume(int value)
{
	if (changing)
	{
		return;
	}

	volume->setValue(value);
}

void VolumeButton::onVolumeValueChanged(int value)
{
	update(value);
}

void VolumeButton::onVolumeSliderPressed()
{
	changing = true;
}

void VolumeButton::onVolumeSliderReleased()
{
	changing = false;
	setSpotifyVolume(volume->value());
}

void VolumeButton::changeVolume(int steps)
{
	const auto value = volume->value() + steps;
	volume->setValue(value);
	setSpotifyVolume(value);
}

void VolumeButton::onVolumeUp(bool /*checked*/)
{
	changeVolume(1);
}

void VolumeButton::onVolumeDown(bool /*checked*/)
{
	changeVolume(-1);
}

void VolumeButton::setSpotifyVolume(int value)
{
	spotify.set_volume(value * step, [](const std::string &status)
	{
		if (!status.empty())
		{
			StatusMessage::error(QString("Failed to set volume: %1")
				.arg(QString::fromStdString(status)));
		}
	});
}

auto VolumeButton::createButton(const QString &text) -> QPushButton *
{
	auto *button = new QPushButton(text, this);
	button->setFlat(true);
	button->setFocusPolicy(Qt::NoFocus);
	button->setFixedWidth(width);

	auto font = button->font();
	font.setPointSize(14);
	button->setFont(font);

	return button;
}
