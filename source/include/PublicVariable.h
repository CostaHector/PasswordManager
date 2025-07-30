#ifndef PUBLICVARIABLE_H
#define PUBLICVARIABLE_H

#include <QSettings>

static inline QSettings& PreferenceSettings() {
  static QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Costa", "PasswordManager");
  return settings;
}

namespace ViewStyleSheet {
QString GetDefaultTableViewStyleSheet();
}

#endif  // PUBLICVARIABLE_H
