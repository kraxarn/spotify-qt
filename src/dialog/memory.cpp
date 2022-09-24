#include "dialog/memory.hpp"
#include "lib/format.hpp"

#include <QPushButton>
#include <QIntValidator>

Dialog::Memory::Memory(QWidget *parent)
	: Base(parent)
{
	auto *layout = Base::layout<QVBoxLayout>();

	status = new QLabel(this);
	layout->addWidget(status);

	allocateMb = new QLineEdit(this);
	allocateMb->setValidator(new QIntValidator(minAllocate, maxAllocate, this));
	layout->addWidget(allocateMb);

	auto *allocate = Base::addButton(QStringLiteral("Allocate"), QDialogButtonBox::ApplyRole);

	QPushButton::connect(allocate, &QPushButton::clicked,
		this, &Dialog::Memory::onAllocateClicked);

	Base::addAction(DialogAction::Ok);
	updateStatus();
}

void Dialog::Memory::updateStatus()
{
	const auto memory = lib::format::size(data.size());
	status->setText(QString("Memory: %1").arg(QString::fromStdString(memory)));
}

void Dialog::Memory::onAllocateClicked(bool /*checked*/)
{
	auto toAllocateMb = allocateMb->text().toInt();
	auto toAllocateB = toAllocateMb * lib::format::mega;
	data.append(toAllocateB, '0');

	lib::log::debug("Allocated {} mb", toAllocateMb);
	updateStatus();
}
