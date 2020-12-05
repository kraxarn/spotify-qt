#pragma once

#include "../mainwindow.hpp"
#include "settings.hpp"

#include <QDialog>
#include <QVBoxLayout>
#include <QtGlobal>

class WhatsNewDialog: public QDialog
{
Q_OBJECT

public:
	WhatsNewDialog(const QString &tag, Settings &settings, QWidget *parent);
};