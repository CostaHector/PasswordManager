#include <QApplication>
#include <QDebug>
#include "PasswordManager.h"
#include "LoginQryWidget.h"
#include "SimpleAES.h"
int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  LoginQryWidget loginDlg;
  QObject::connect(&loginDlg, &LoginQryWidget::accepted, &loginDlg, [&loginDlg]() {
    bool bFromEncrypt{true};
    QString key;
    std::tie(bFromEncrypt, key) = loginDlg.GetEncryptAndKey();
    qDebug("key length[%d] bFromEncrypt[%d]", key.size(), bFromEncrypt);
    SimpleAES::setKey(key);
    SimpleAES::setFromEncrypt(bFromEncrypt);
    PasswordManager* pw = new PasswordManager;
    pw->show();
  });
  loginDlg.show();
  return a.exec();
}
