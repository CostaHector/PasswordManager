#ifndef PWDTABLEMODEL_H
#define PWDTABLEMODEL_H

#include <QSortFilterProxyModel>
#include "AccountStorage.h"
#include "PublicVariable.h"
#include "QAbstractTableModelPub.h"
#include <set>

class AccountSortFilterProxyModel : public QSortFilterProxyModel {
public:
  using QSortFilterProxyModel::QSortFilterProxyModel;
  void BindAccountsList(const AccountStorage& accountsList) { pAccountsList = &accountsList; }

protected:
  bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
  const AccountStorage* pAccountsList{nullptr};
};

class PwdTableModel : public QAbstractTableModelPub {
public:
  friend class AccountListView;
  friend class AccountSortFilterProxyModel;
  explicit PwdTableModel(QObject* parent = nullptr);
  int rowCount(const QModelIndex& parent = {}) const override { return mAccountsList.size(); }
  int columnCount(const QModelIndex& parent = {}) const override {
    return AccountInfo::HORIZONTAL_HEAD.size();
  }

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
        return AccountInfo::HORIZONTAL_HEAD[section];
      }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
  }

  Qt::ItemFlags flags(const QModelIndex& index) const override {
    return Qt::ItemFlag::ItemIsEditable | Qt::ItemFlag::ItemIsEnabled
           | Qt::ItemFlag::ItemIsSelectable;
  }

  int RemoveIndexes(const std::set<int>& rows);
  bool InsertNRows(int indexBefore, int cnt);
  int AppendAccountRecords(const QVector<AccountInfo>& tempAccounts);

  void subscribe();
  SAVE_RESULT onSave(QString* detailMessage = nullptr);
  QString GetExportCSVRecords() const { return mAccountsList.GetExportCSVRecords(); }
  bool IsDirty() const { return mAccountsList.IsDataDirty(); }
  bool ExportToPlainCSV() const;
  AccountInfo* rowDataAt(int index);
  bool GetLoadResult() const { return mLoadResult; }

private:
  AccountStorage mAccountsList;
  QMap<QString, QIcon> mType2Icon;
  QMap<QString, QString> mType2CardTemplate;
  bool mIsDirty{false};
  bool mLoadResult{false};
};

#endif // PWDTABLEMODEL_H
