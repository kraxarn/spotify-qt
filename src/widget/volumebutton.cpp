#include "volumebutton.hpp"
#include "mainwindow.hpp"
#include "util/shortcut.hpp"
#include "widget/clickableslider.hpp"

VolumeButton::VolumeButton(lib::settings &settings, lib::spt::api &spotify, QWidget *parent)
	: QToolButton(parent),
	settings(settings),
	spotify(spotify)
{
	// Volume slider
	volume = new ClickableSlider(Qt::Vertical, this);
	volume->setFixedHeight(height);
	volume->setFixedWidth(width);
	volume->setMinimum(minimum);
	volume->setMaximum(maximum);

	// Initially assume full volume until refreshed
	volume->setValue(maximum);

	// Layout for volume slider
	auto *volumeMenu = new QMenu(this);

	volumeUp = createButton(QStringLiteral("+"));
	volumeUp->setShortcut(Shortcut::volumeUp());
	QPushButton::connect(volumeUp, &QPushButton::clicked,
		this, &VolumeButton::onVolumeUp);

	volumeDown = createButton(QStringLiteral("-"));
	volumeDown->setShortcut(Shortcut::volumeDown());
	QPushButton::connect(volumeDown, &QPushButton::clicked,
		this, &VolumeButton::onVolumeDown);

	auto *volumeLayout = new QVBoxLayout();
	volumeLayout->addWidget(volumeUp);
	volumeLayout->addWidget(volume);
	volumeLayout->addWidget(volumeDown);
	volumeMenu->setLayout(volumeLayout);

	setText(QStringLiteral("Volume"));
	update(volume->value());
	setPopupMode(QToolButton::InstantPopup);
	setMenu(volumeMenu);

	QAbstractSlider::connect(volume, &QAbstractSlider::valueChanged,
		this, &VolumeButton::onVolumeValueChanged);

	QAbstractSlider::connect(volume, &QAbstractSlider::sliderPressed,
		this, &VolumeButton::onVolumeSliderPressed);

	QAbstractSlider::connect(volume, &QAbstractSlider::sliderReleased,
		this, &VolumeButton::onVolumeSliderReleased);
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

	settings.spotify.volume = value * step;
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
