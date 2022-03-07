#include "view/context/abstractcontent.hpp"

Context::AbstractContent::AbstractContent(QWidget *parent)
	: QWidget(parent)
{
}

auto Context::AbstractContent::getCurrentlyPlaying() const -> const lib::spt::track &
{
	return currentlyPlaying;
}