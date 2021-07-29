#include "view/artist/title.hpp"

View::Artist::Title::Title(QWidget *parent)
	: QLabel("...", parent)
{
	setWordWrap(true);

	// Show loading text
	new Loader(this);

	// Larger text
	auto titleFont = font();
	titleFont.setPointSize(pointSize);
	setFont(titleFont);

	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);
}
