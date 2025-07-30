#include "include/PwdTableModel.h"
#include "include/TableEditActions.h"
#include "include/CardTemplate.h"
#include <QDateTime>
#include <QFile>
#include <QIcon>
#include <set>
#include <QMessageBox>
#include <QInputDialog>

bool LoadTypeIcons(QMap<QString, QIcon>& type2Icons) {
  type2Icons["unionpay"] = QIcon(":/bankcard/UNIONPAY");
  type2Icons["mastercard"] = QIcon(":/bankcard/MASTERCARD");
  type2Icons["visa"] = QIcon(":/bankcard/VISA");
  return true;
}

PwdTableModel::PwdTableModel(QObject* parent) : QAbstractTableModelPub{parent} {
  if (!LoadTypeIcons(mType2Icon)) {
    qWarning("Load Type Icons failed");
  }
  mType2CardTemplate = InitCardTemplate();
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
        return mAccountsList[index.row()].Issuer;
      case 2:
        return mAccountsList[index.row()].Type;
      case 3:
        return mAccountsList[index.row()].Account;
      case 4:
        return mAccountsList[index.row()].Withdraw;
      case 5:
        return mAccountsList[index.row()].CVV2;
      case 6:
        return mAccountsList[index.row()].ValidThrough;
      case 7:
        return mAccountsList[index.row()].CardHolder;
      case 8:
        return mAccountsList[index.row()].AnnualFee;
      case 9:
        return mAccountsList[index.row()].Notes;
      case 10:
        return QDateTime::fromSecsSinceEpoch(mAccountsList[index.row()].RecordTime);
      case 11:
        return mAccountsList[index.row()].QueryCode;
      default:
        return QVariant();
    }
  } else if (role == Qt::ToolTipRole) {
    auto it = mType2CardTemplate.constFind(mAccountsList[index.row()].Type);
    if (it == mType2CardTemplate.constEnd()) {
      return {};
    }
    const auto& info = mAccountsList[index.row()];
    const QString svgContent = it.value().arg(info.Account).arg(info.ValidThrough).arg(info.CVV2).arg(info.Issuer).arg(info.Notes);
    return "<img src=\"data:image/svg;base64, " + svgContent.toUtf8().toBase64() + "\"/>";
  } else if (role == Qt::TextAlignmentRole) {
    return int(Qt::AlignLeft);
  } else if (role == Qt::DecorationRole) {
    if (index.column() == 2) {
      return mType2Icon.value(mAccountsList[index.row()].Type.toLower(), {});
    }
  }
  return {};
}

bool PwdTableModel::setData(const QModelIndex& index, const QVariant& value, int role) {
  if (role == Qt::EditRole) {  // 0: value
    const int section = index.column();
    switch (section) {
      case 1:
        mAccountsList[index.row()].Issuer = value.toString();
        break;
      case 2:
        mAccountsList[index.row()].Type = value.toString();
        break;
      case 3:
        mAccountsList[index.row()].Account = value.toString();
        break;
      case 4:
        mAccountsList[index.row()].Withdraw = value.toString();
        break;
      case 5:
        mAccountsList[index.row()].CVV2 = value.toInt();
        break;
      case 6:
        mAccountsList[index.row()].ValidThrough = value.toString();
        break;
      case 7:
        mAccountsList[index.row()].CardHolder = value.toString();
        break;
      case 8:
        mAccountsList[index.row()].AnnualFee = value.toInt();
        break;
      case 9:
        mAccountsList[index.row()].Notes = value.toString();
        break;
      case 10:
        mAccountsList[index.row()].RecordTime = value.toInt();
        break;
      case 11:
        mAccountsList[index.row()].QueryCode = value.toString();
        break;
      default:
        return false;
    }
    mIsDirty = true;
    emit dataChanged(index, index, {Qt::DisplayRole});
  }
  return true;
  ;
}

int PwdTableModel::RemoveIndexes(const std::set<int>& rows) {
  if (rows.empty()) {
    qDebug("no row need to be delete");
    return true;
  }

  int before = rowCount();
  auto newAccountsList{mAccountsList};
  int rowsDeleted = newAccountsList.RemoveIndexes(rows);
  if (rowsDeleted == 0) {
    qDebug("no row deleted, no need change row count");
    return true;
  }
  int after = newAccountsList.size();

  RowsCountStartChange(before, after);
  mAccountsList.swap(newAccountsList);
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

  int before = rowCount();
  auto newAccountsList{mAccountsList};
  if (!newAccountsList.InsertNRows(indexBefore, cnt)) {
    qDebug("insert before index[%d] %d rows failed", indexBefore, cnt);
    return false;
  }
  int after = newAccountsList.size();

  RowsCountStartChange(before, after);
  mAccountsList.swap(newAccountsList);
  RowsCountEndChange(before, after);
  return true;
}

void PwdTableModel::subscribe() {
  auto& ins = GetTableEditActionsInst();
  connect(ins.SAVE_CHANGES, &QAction::triggered, this, &PwdTableModel::onSave);
  connect(ins.CREATE_A_TABLE, &QAction::triggered, this, &PwdTableModel::onNewTable);
}

bool PwdTableModel::onNewTable() {
  QFile jsonFile{JsonReader::PDBOOK_TABLE_NAME};
  if (jsonFile.exists()) {
    qDebug("file[%s] already exist", qPrintable(jsonFile.fileName()));
    return false;
  }

  bool ok = false;
  const QString& key16 = QInputDialog::getText(nullptr, "Encrypt key set",
                                               "Set an encrypt key and keep in mind.<br/>"
                                               "Don't let anyone know it",
                                               QLineEdit::EchoMode::Normal, "", &ok);
  if (!ok) {
    qDebug("User cancel create a new table");
    return false;
  }

  if (!jsonFile.open(QIODevice::NewOnly | QIODevice::WriteOnly | QIODevice::Text)) {
    jsonFile.close();
    qWarning("Create a new file[%s] to write failed", qPrintable(JsonReader::PDBOOK_TABLE_NAME));
    return false;
  }
  jsonFile.write("[]");
  jsonFile.close();
  qInfo("PDBook Create in path[%s] succeed", qPrintable(jsonFile.fileName()));
  SetKey16(key16);
  return true;
}

bool PwdTableModel::SetKey16(const QString& key16) {
  int before = rowCount();
  JsonReader newAccountsList{mAccountsList};
  bool key16DecryptedResult = newAccountsList(key16);
  if (!key16DecryptedResult) {
    qDebug("Change key16 and reload json file failed");
  }
  int after = newAccountsList.size();
  RowsCountStartChange(before, after);
  mAccountsList.swap(newAccountsList);
  RowsCountEndChange(before, after);
  return key16DecryptedResult;
}

bool PwdTableModel::SetEncryptKey16(const QString& newKey16) {
  return mAccountsList.SetEncryptKey16(newKey16);
}

void PwdTableModel::onSave() {
  const int recordCnt = mAccountsList.size();
  if (!mAccountsList.IsJsonFileEmpty() && recordCnt == 0) {
    QMessageBox::critical(nullptr, "Save rejected.", "Table has not been loaded. Load before saving.");
    return;
  }

  if (mAccountsList.IsEncryptKeyDiffersFromDecryptKey()) {
    auto btn = QMessageBox::critical(nullptr, "Confirm to use new encrypt key to save?",
                                     "It will immediately activate the new encrypt key. "
                                     "Irreversibly <b>re-encrypt all data using the new encrypt key</b>. "
                                     "Upon confirmation, <b>the legacy key becomes invalid</b>. "
                                     "<b>All future operations require the new encryption key</b>.",
                                     QMessageBox::StandardButton::Save | QMessageBox::StandardButton::Cancel,  // candidate
                                     QMessageBox::StandardButton::Cancel);
    if (btn != QMessageBox::StandardButton::Save) {
      qWarning("[Encrypt/Decrypt key16 differ] User cancel save");
      return;
    }
  }

  const bool saveResult = mAccountsList.Save();
  const QString title{QString::number(recordCnt) + "record(s)"};
  if (saveResult) {
    mIsDirty = false;
    QMessageBox::information(nullptr, title, "All succeed saved to " + JsonReader::PDBOOK_TABLE_NAME);
  } else {
    QMessageBox::warning(nullptr, title, "Failed save into " + JsonReader::PDBOOK_TABLE_NAME);
  }
}
