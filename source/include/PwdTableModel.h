#ifndef PWDTABLEMODEL_H
#define PWDTABLEMODEL_H

#include "QAbstractTableModelPub.h"
#include "JsonReader.h"
#include <set>

class PwdTableModel : public QAbstractTableModelPub
{
 public:
  friend class PasswordTable;
  explicit PwdTableModel(QObject* parent = nullptr);
  int rowCount(const QModelIndex& parent = {}) const override { return mAccountsList.size(); }
  int columnCount(const QModelIndex& parent = {}) const override { return HORIZONTAL_HEAD.size(); }

  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

  QVariant headerData(int section, Qt::Orientation orientation, int role) const override {
    if (role == Qt::TextAlignmentRole) {
      if (orientation == Qt::Vertical) {
        return Qt::AlignRight;
      }
    }
    if (role == Qt::DisplayRole) {
      if (orientation == Qt::Orientation::Vertical) {
        return section + 1;
      } else {
        return HORIZONTAL_HEAD[section];
      }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
  }

  Qt::ItemFlags flags(const QModelIndex& index) const override {
    return Qt::ItemFlag::ItemIsEditable | Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable;
  }

  int RemoveIndexes(const std::set<int>& rows);
  bool InsertNRows(int indexBefore, int cnt);
  void subscribe();
  bool SetKey16(const QString& key16);
  bool SetEncryptKey16(const QString& newKey16);
  void onSave();
  bool onNewTable();
  bool IsDirty() const {
    return mIsDirty;
  }
  bool IsDecryptKeyCorrect() const {
    return mAccountsList.IsDecryptKeyCorrect();
  }
  bool IsEncryptKeyCorrect() const {
    return mAccountsList.isEncryptKeyLengthValid();
  }
 private:
  JsonReader mAccountsList;
  QMap<QString, QIcon> mType2Icon;
  QMap<QString, QString> mType2CardTemplate;
  bool mIsDirty {false};
};

#endif // PWDTABLEMODEL_H
