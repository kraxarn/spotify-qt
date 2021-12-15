#include "dialog/base.hpp"

#include "util/widget.hpp"

Dialog::Base::Base(lib::settings &settings, QWidget *parent)
	: QDialog(parent),
	settings(settings)
{
	setWindowFlag(Qt::FramelessWindowHint, !settings.qt().system_title_bar);
	setModal(true);

	layout = new QVBoxLayout(this);

	toolBar = new QToolBar(this);
	toolBar->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(toolBar);

	title = new QLabel(this);
	toolBar->addWidget(title);

	auto *dragArea = new DragArea(this);
	dragArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	dragArea->setMinimumWidth(100);
	toolBar->addWidget(dragArea);
}

void Dialog::Base::onOk(bool /*checked*/)
{
	close();
}

void Dialog::Base::onApply(bool /*checked*/)
{
}

void Dialog::Base::onCancel(bool /*checked*/)
{
	close();
}

void Dialog::Base::addLayout(QLayout *content)
{
	auto *frame = Widget::toFrame(content, this);
	frame->setContentsMargins(contentMargins, contentMargins, contentMargins, contentMargins);
	frame->setFrameShape(QFrame::StyledPanel);
	layout->addWidget(frame, 1);
}

void Dialog::Base::addActions(DialogAction dialogAction)
{
	if (dialogAction == DialogAction::Ok && ok == nullptr)
	{
		ok = toolBar->addAction(Icon::get("dialog-ok"), "OK");
		QAction::connect(ok, &QAction::triggered,
			this, &Dialog::Base::onOk);
	}
	else if (dialogAction == DialogAction::Apply && apply == nullptr)
	{
		apply = toolBar->addAction(Icon::get("document-save"), "Apply");
		QAction::connect(apply, &QAction::triggered,
			this, &Dialog::Base::onApply);
	}
	else if (dialogAction == DialogAction::Cancel && cancel == nullptr)
	{
		cancel = toolBar->addAction(Icon::get("dialog-cancel"), "Cancel");
		QAction::connect(cancel, &QAction::triggered,
			this, &Dialog::Base::onCancel);
	}
}

void Dialog::Base::setTitle(const QString &text)
{
	title->setText(text);
}
