#include "dialog/about.hpp"
#include "widget/about.hpp"

Dialog::About::About(QWidget *parent)
	: Base(parent)
{
	auto *layout = Base::layout<QVBoxLayout>();

	auto *about = new ::About(this);
	layout->addWidget(about);

	Base::addAction(DialogAction::Ok);
}
