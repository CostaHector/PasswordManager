#ifndef PUBLICVARIABLE_H
#define PUBLICVARIABLE_H

#include <QSettings>
#include <QFont>
static inline QSettings& PreferenceSettings() {
  static QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Costa", "PasswordManager");
  return settings;
}

namespace ViewStyleSheet {
QString GetDefaultTableViewStyleSheet();
extern const QFont TEXT_EDIT_FONT;
constexpr char SUBMIT_BTN_STYLE[] //
    {"QPushButton{"
     "    color: #fff;"
     "    background-color: DodgerBlue;"
     "    border-color: DodgerBlue;"
     "}"
     "QPushButton:hover {"
     "    color: #fff;"
     "    background-color: rgb(36, 118, 199);"
     "    border-color: rgb(36, 118, 199);"
     "}"};
}

#endif  // PUBLICVARIABLE_H
