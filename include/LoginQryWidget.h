#ifndef LOGIN_QRY_WIDGET_H
#define LOGIN_QRY_WIDGET_H
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QStackedLayout>
#include <QVBoxLayout>
class LoginQryWidget : public QDialog {
public:
  explicit LoginQryWidget(QWidget *parent = nullptr);
  QString getAESKey() const { return mKey; }
  void Subscribe();

private:
  QLineEdit *CreateKeyLineEdit() const;
  QLabel *CreateMessageLabel() const;

  QWidget *CreateLoginPage();
  QWidget *CreateRegisterPage();

  QString mKey;

  QWidget *mLoginWid{nullptr}, *mRegisterWid{nullptr};
  QTabBar *mLoginRegisterTab{nullptr};
  QStackedLayout *mLoginRegisterStkLo{nullptr};

  QVBoxLayout *mMainLayout{nullptr};
};
#endif
