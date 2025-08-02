#ifndef LOGIN_QRY_WIDGET_H
#define LOGIN_QRY_WIDGET_H
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QLabel>
class LoginQryWidget : public QDialog {
public:
  explicit LoginQryWidget(QWidget *parent = nullptr);
  QString getAESKey() const { return mKey; }
  bool isEncryptedSource() const { return mLoginRegisterStkLo->currentIndex() == ENCRYPT; }
  void Subscribe();
  std::pair<bool, QString> GetEncryptAndKey() const;
private:
  QLineEdit* CreateKeyLineEdit() const;
  QLabel* CreateMessageLabel() const;
  enum DataSourceType { BEGIN = 0, ENCRYPT = BEGIN, PLAIN, BUTT };

  QWidget *CreateLoginPage();
  QWidget *CreateRegisterPage();

  QString mKey;

  QWidget *mLoginWid{nullptr}, *mRegisterWid{nullptr};
  QTabBar *mLoginRegisterTab{nullptr};
  QStackedLayout *mLoginRegisterStkLo{nullptr};

  QVBoxLayout* mMainLayout{nullptr};
};
#endif
