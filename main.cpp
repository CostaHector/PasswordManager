#include <QApplication>
#include <QDebug>
#include "PasswordManager.h"
#include "LoginQryWidget.h"
#include "SimpleAES.h"
int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  LoginQryWidget loginDlg;
  QObject::connect(&loginDlg, &LoginQryWidget::accepted, &loginDlg, [&loginDlg]() {
    QString key = loginDlg.getAESKey();
    qDebug("key length[%d]", key.size());
    SimpleAES::setKey(key);
    PasswordManager* pw = new PasswordManager;
    pw->show();
  });
  loginDlg.show();
  return a.exec();
}
