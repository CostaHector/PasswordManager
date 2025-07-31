#include "TableEditActions.h"

TableEditActions::TableEditActions(QObject* parent) : QObject{parent} {
  INSERT_A_ROW = new QAction{QIcon{":/edit/INSERT_ROW_BEFORE"}, "Insert a row"};
  INSERT_A_ROW->setToolTip("Insert a row before current line");
  INSERT_ROWS = new QAction{QIcon{":/edit/INSERT_ROWS"}, "Insert rows"};
  INSERT_ROWS->setToolTip("Insert row(s) before current line");
  APPEND_ROWS = new QAction{QIcon{":/edit/APPEND_ROWS"}, "Append rows"};
  APPEND_ROWS->setToolTip("Append row(s) behind last row");
  DELETE_ROWS = new QAction{QIcon{":/edit/DELETE_ROWS"}, "Delete rows"};
  DELETE_ROWS->setToolTip("Delete row(s) selected currently");
  EXPORT_TO_PLAIN_CSV = new QAction{QIcon{":/edit/EXPORT"}, "Export Plain CSV"};
  EXPORT_TO_PLAIN_CSV->setToolTip("Export account table records to CSV file in plaintext (<b>unencrypted, human-readable</b>)");

  ROW_EDIT_AG = new QActionGroup{this};
  ROW_EDIT_AG->addAction(INSERT_A_ROW);
  ROW_EDIT_AG->addAction(INSERT_ROWS);
  ROW_EDIT_AG->addAction(APPEND_ROWS);
  ROW_EDIT_AG->addAction(DELETE_ROWS);
  ROW_EDIT_AG->addAction(EXPORT_TO_PLAIN_CSV);

  SAVE_CHANGES = new QAction{QIcon{":/edit/SAVE_CHANGES"}, "Save changes"};
  SAVE_CHANGES->setShortcut(QKeySequence(Qt::Key::Key_F10));
  SAVE_CHANGES->setToolTip(QString("<b>%1 (%2)</b><br/>Save All changes to local file.")
                               .arg(SAVE_CHANGES->text(), SAVE_CHANGES->shortcut().toString()));
}

TableEditActions& GetTableEditActionsInst() {
  static TableEditActions ins;
  return ins;
}
