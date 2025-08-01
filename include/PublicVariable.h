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
     "}"
     "QPushButton:hover {"
     "    color: #fff;"
     "    background-color: rgb(36, 118, 199);"
     "}"};
}

enum class SAVE_RESULT {
  OK = 0,
  SKIP = 1,
  FAILED = 2,
  BUTT
};
constexpr char SAVE_RESULT_STR[(int)SAVE_RESULT::BUTT][10]{
  "OK", "SKIP", "FAILED"
};

#endif  // PUBLICVARIABLE_H
