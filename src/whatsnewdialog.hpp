#pragma once

#include "mainwindow.hpp"
#include "settings.hpp"

#include <QtGlobal>
#include <QDialog>
#include <QVBoxLayout>

class WhatsNewDialog : public QDialog
{
	Q_OBJECT

public:
	WhatsNewDialog(const QString &tag, QWidget *parent);
};