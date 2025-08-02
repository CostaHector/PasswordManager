#ifndef WINCREDUTIL_H
#define WINCREDUTIL_H

#ifdef _WIN32
// clang-format off
#include <windows.h>
#include <wincred.h>
// clang-format on
#include <QString>

class WinCredUtil {
public:
  static bool savePassword(const QString &key, const QString &password);
  static QString readPassword(const QString &key);
  static bool deletePassword(const QString &key);

private:
  static bool credentialExists(const QString &key);
};
#endif

#endif
