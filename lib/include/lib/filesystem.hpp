#pragma once

// Workaround for
// https://bugreports.qt.io/browse/QTBUG-73263

#ifdef QT_CORE_LIB
#include <QCoreApplication>
#endif

#include <filesystem> // NOLINT(*-include-cleaner)
