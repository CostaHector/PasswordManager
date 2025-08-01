#ifndef CSVINPUTDIALOG_H
#define CSVINPUTDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include "AccountStorage.h"

class CSVInputDialog : public QDialog {
public:
  explicit CSVInputDialog(QWidget *parent = nullptr);
  QSize sizeHint() const override {
    return QSize{1024, 720};
  }
  void raise();
  QVector<AccountInfo> tempAccounts;

private:
  void onHelpRequest();
  QTextEdit *textEdit{nullptr};
  QDialogButtonBox *buttonBox{nullptr};
  QVBoxLayout *mainLayout{nullptr};
};

#endif // CSVINPUTDIALOG_H
