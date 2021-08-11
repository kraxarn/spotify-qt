#include "nowplaying.hpp"

View::Context::NowPlaying::NowPlaying(QWidget *parent)
	: QWidget(parent)
{
	layout = new QVBoxLayout(this);
	layout->setSpacing(0);

	name = newLabel(nameScale);
	artist = newLabel(artistScale);
}

void View::Context::NowPlaying::setTrack(const lib::spt::track &track)
{
	if (name != nullptr)
	{
		name->setVisible(true);
		setText(name, track.name);
	}

	if (artist != nullptr)
	{
		const auto names = lib::spt::entity::combine_names(track.artists);
		setText(artist, names);
	}
}

void View::Context::NowPlaying::setNoPlaying()
{
	if (name != nullptr)
	{
		name->setVisible(false);
	}

	if (artist != nullptr)
	{
		setText(artist, "No music playing");
	}
}

auto View::Context::NowPlaying::newLabel(float scale) -> QLabel *
{
	auto *label = new QLabel(this);
	label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);

	auto font = label->font();
	auto pointSize = static_cast<float>(font.pointSize()) * scale;
	font.setPointSize(static_cast<int>(pointSize));
	label->setFont(font);

	layout->addWidget(label);
	return label;
}

void View::Context::NowPlaying::setText(QLabel *label, const std::string &text)
{
	auto qText = QString::fromStdString(text);
	label->setText(qText);
	label->setToolTip(qText);
}
