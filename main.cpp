#include "PasswordManager.h"

#include <QApplication>
#include <QDebug>
#include <QCryptographicHash>

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  PasswordManager w;
  w.show();
  return a.exec();
}
