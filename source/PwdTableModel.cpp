#include "PwdTableModel.h"
#include <QFile>
#include <QIcon>
#include <QInputDialog>
#include <QMessageBox>
#include "CardTemplate.h"
#include <set>

bool AccountSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  const QString keyword = filterRegExp().pattern(); // Qt 5
#else
  const QString keyword = filterRegularExpression().pattern(); // Qt 6
#endif
  if (pAccountsList == nullptr || keyword.isEmpty()) {
    return true;
  }
  return (*pAccountsList)[sourceRow].IsContainsKeyWords(keyword);
}

bool LoadTypeIcons(QMap<QString, QIcon>& type2Icons) {
  type2Icons["unionpay"] = QIcon(":/bankcard/UNIONPAY");
  type2Icons["mastercard"] = QIcon(":/bankcard/MASTERCARD");
  type2Icons["visa"] = QIcon(":/bankcard/VISA");
  return true;
}

PwdTableModel::PwdTableModel(QObject* parent)
  : QAbstractTableModelPub{parent} {
  if (!LoadTypeIcons(mType2Icon)) {
    qWarning("Load Type Icons failed");
  }
  mType2CardTemplate = InitCardTemplate();
  mLoadResult = mAccountsList.LoadAccounts();
  subscribe();
}

QVariant PwdTableModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid()) {
    return QVariant();
  }
  if (!(0 <= index.row() && index.row() < rowCount())) {
    return QVariant();
  }
  if (role == Qt::DisplayRole || role == Qt::EditRole) {
    const int section = index.column();
    switch (section) {
      case 0:
        return index.row();
      case 1:
        return mAccountsList[index.row()].typeStr;
      case 2:
        return mAccountsList[index.row()].nameStr;
      case 3:
        return mAccountsList[index.row()].accountStr;
      default:
        return QVariant();
    }
  } else if (role == Qt::TextAlignmentRole) {
    return int(Qt::AlignLeft);
  } else if (role == Qt::DecorationRole) {
    if (index.column() == 1) {
      return mType2Icon.value(mAccountsList[index.row()].typeStr.toLower(), {});
    }
  }
  return {};
}

bool PwdTableModel::setData(const QModelIndex& index, const QVariant& value, int role) {
  if (role == Qt::EditRole) { // 0: value
    const int section = index.column();
    switch (section) {
      case 1:
        mAccountsList[index.row()].typeStr = value.toString();
        break;
      case 2:
        mAccountsList[index.row()].nameStr = value.toString();
        break;
      case 3:
        mAccountsList[index.row()].accountStr = value.toString();
        break;
      default:
        return false;
    }
    mAccountsList[index.row()].SetDetailModified();
    emit dataChanged(index, index, {Qt::DisplayRole});
  }
  return true;
}

int PwdTableModel::RemoveIndexes(const std::set<int>& rows) {
  if (rows.empty()) {
    qDebug("no row need to be delete");
    return 0;
  }
  const int cnt = rows.size();
  const int before = rowCount();
  if (cnt > before) {
    qWarning("delete row(s) count %d > total count %d", cnt, before);
    return -1;
  }
  const int after = before - cnt;
  RowsCountStartChange(before, after);
  int rowsDeleted = mAccountsList.RemoveIndexes(rows);
  qDebug("%d/%d row(s) were deleted", rowsDeleted, cnt);
  RowsCountEndChange(before, after);
  return rowsDeleted;
}

bool PwdTableModel::InsertNRows(int indexBefore, int cnt) {
  if (cnt == 0) {
    qDebug("No row to insert");
    return true;
  }
  if (indexBefore < 0) {
    indexBefore = 0;
  }
  if (indexBefore >= rowCount()) {
    indexBefore = rowCount();
  }
  const int before = rowCount();
  const int after = before + cnt;
  RowsCountStartChange(before, after);
  if (!mAccountsList.InsertNRows(indexBefore, cnt)) {
    qDebug("insert before index[%d] %d rows failed", indexBefore, cnt);
    return false;
  }
  RowsCountEndChange(before, after);
  return true;
}

int PwdTableModel::AppendAccountRecords(const QVector<AccountInfo>& tempAccounts) {
  int cnt = tempAccounts.size();
  if (cnt == 0) {
    qDebug("No records to insert");
    return true;
  }

  int before = rowCount();
  int after = before + tempAccounts.size();

  RowsCountStartChange(before, after);
  mAccountsList += tempAccounts;
  RowsCountEndChange(before, after);
  return true;
}

void PwdTableModel::subscribe() {}

bool PwdTableModel::ExportToPlainCSV() const {
  return mAccountsList.SaveAccounts(false);
}

SAVE_RESULT PwdTableModel::onSave(QString* detailMessage) {
  if (!IsDirty()) {
    qDebug("Nothing changed. No need to save at all");
    return SAVE_RESULT::SKIP;
  }
  QString detailMsg = mAccountsList.GetRowChangeDetailMessage();
  bool saveResult = mAccountsList.SaveAccounts(true);
  if (!saveResult) {
    qWarning("Save record(s) failed");
    return SAVE_RESULT::FAILED;
  }
  if (detailMessage != nullptr) {
    *detailMessage = detailMsg;
  }
  qDebug("Save [%s] succeed.", qPrintable(detailMsg));
  return SAVE_RESULT::OK;
}

AccountInfo* PwdTableModel::rowDataAt(int index) {
  if (index < 0 || index > rowCount()) {
    return nullptr;
  }
  return &mAccountsList[index];
}
