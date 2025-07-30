#include "CardTemplate.h"
#include <QIODevice>
#include <QFile>

QMap<QString, QString> InitCardTemplate() {
  static const QMap<QString, QString> resourceTemplate{{"VISA", ":/bankcard/VISA_CARD_TEMPLATE"}, {"UNIONPAY", ":/bankcard/UNION_CARD_TEMPLATE"}, {"MASTERCARD", ":/bankcard/MASTER_CARD_TEMPLATE"}};
  QMap<QString, QString> card2TemplateString;
  for (auto it = resourceTemplate.cbegin(); it != resourceTemplate.cend(); ++it) {
    QFile fi{it.value()};
    if (!fi.open(QIODevice::OpenModeFlag::Text | QIODevice::OpenModeFlag::ReadOnly)) {
      qWarning("Open file[%s] failed", qPrintable(fi.fileName()));
      return {};
    }
    card2TemplateString[it.key()] = fi.readAll();
    fi.close();
  }
  return card2TemplateString;
}
