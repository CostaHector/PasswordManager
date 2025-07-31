#ifndef LOGIN_QRY_WIDGET_H
#define LOGIN_QRY_WIDGET_H
#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QVBoxLayout>

class LoginQryWidget : public QDialog {
public:
  explicit LoginQryWidget(QWidget *parent = nullptr);
  QString getAESKey() const {
    return mKeyEdit->text();
  }
  bool isEncryptedSource() const {
    return mSourceCombo->currentIndex() == DataSourceType::ENCRYPT;
  }
  void Subscribe();
  std::pair<bool, QString> GetEncryptAndKey() const;
  void onSourceChanged(int index);
  void validateInput();

private:
  void updateTitle(bool isMismatch);
  enum DataSourceType{ENCRYPT, PLAIN};

  QComboBox *mSourceCombo{nullptr};
  QLineEdit *mKeyEdit{nullptr};
  QLineEdit *mConfirmKeyEdit{nullptr};
  QDialogButtonBox* mButtonBox{nullptr};
  QVBoxLayout *mMainLayout{nullptr};
};
#endif
