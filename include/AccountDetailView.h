#ifndef ACCOUNTDETAILVIEW_H
#define ACCOUNTDETAILVIEW_H

#include <QDockWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <functional>
#include "AccountStorage.h"

typedef std::function<void()> SET_TABLE_DIRTY ;

class AccountDetailView : public QDockWidget {
public:
  using QDockWidget::QDockWidget;
  explicit AccountDetailView(const QString &title, QWidget *parent = nullptr);
  void UpdateDisplay(AccountInfo *pAcc);
  void onRecoverModify();
  void onApplyModify();
  void Subscribe();
  void BindSetTableDirtyCallback(SET_TABLE_DIRTY setTableDirtyFunc) const {
    pSetTableDirty = setTableDirtyFunc;
  }
private:
  void editHappen();
  void editNotHappen();

  QLineEdit *etType{nullptr}, *etName{nullptr}, *etAccount{nullptr}, *etPwd{nullptr};
  QTextEdit *etOthers{nullptr};
  QPushButton *mBtnRecover{nullptr}, *mBtnApply{nullptr};
  QFormLayout *mForm{nullptr};
  QWidget *mFormWid{nullptr};

  mutable AccountInfo *pAccount{nullptr};
  mutable SET_TABLE_DIRTY pSetTableDirty{nullptr};
};

#endif // ACCOUNTDETAILVIEW_H
