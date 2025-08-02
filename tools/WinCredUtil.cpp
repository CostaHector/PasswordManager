#include "WinCredUtil.h"

bool WinCredUtil::credentialExists(const QString &key) {
  PCREDENTIALW pcred;
  return CredReadW(key.toStdWString().c_str(), CRED_TYPE_GENERIC, 0, &pcred);
}

bool WinCredUtil::savePassword(const QString &key, const QString &password) {
  // 转换字符串为Windows宽字符格式
  std::wstring wkey = key.toStdWString();
  std::wstring wuser = L"User/Aria"; // 可以改为你的应用特定用户标识

  // 准备凭证结构
  CREDENTIALW cred = {0};
  cred.Flags = 0;
  cred.Type = CRED_TYPE_GENERIC;
  cred.TargetName = (LPWSTR) wkey.c_str();
  cred.CredentialBlobSize = password.toUtf8().size();
  cred.CredentialBlob = (LPBYTE) password.toUtf8().data();
  cred.Persist = CRED_PERSIST_LOCAL_MACHINE; // 持久化到本地机器
  cred.UserName = (LPWSTR) wuser.c_str();

  // 保存凭证
  if (credentialExists(key)) {
    // 如果已存在则先删除
    if (!CredDeleteW(wkey.c_str(), CRED_TYPE_GENERIC, 0)) {
      qWarning("Failed to delete existing credential");
      return false;
    }
  }

  return CredWriteW(&cred, 0);
}

QString WinCredUtil::readPassword(const QString &key) {
  PCREDENTIALW pcred;
  std::wstring wkey = key.toStdWString();

  if (CredReadW(wkey.c_str(), CRED_TYPE_GENERIC, 0, &pcred)) {
    QString password = QString::fromUtf8((char *) pcred->CredentialBlob, pcred->CredentialBlobSize);
    CredFree(pcred);
    return password;
  }

  return QString();
}

bool WinCredUtil::deletePassword(const QString &key) {
  std::wstring wkey = key.toStdWString();
  return CredDeleteW(wkey.c_str(), CRED_TYPE_GENERIC, 0);
}
