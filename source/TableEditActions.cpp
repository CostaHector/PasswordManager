#include "include/TableEditActions.h"

TableEditActions::TableEditActions(QObject* parent) : QObject{parent} {
  INSERT_A_ROW = new QAction{QIcon{":/edit/INSERT_A_ROW"}, "Insert a row"};
  INSERT_ROWS = new QAction{QIcon{":/edit/INSERT_ROWS"}, "Insert rows"};
  APPEND_ROWS = new QAction{QIcon{":/edit/APPEND_ROWS"}, "Append rows"};
  DELETE_ROWS = new QAction{QIcon{":/edit/DELETE_ROWS"}, "Delete rows"};
  CREATE_A_TABLE = new QAction{QIcon{":/edit/CREATE_TABLE"}, "Create a table"};

  CREATE_A_TABLE->setToolTip(QString("<b>%1 (%2)</b><br/>"
                                     "Create a table if there is no table.")
                                 .arg(CREATE_A_TABLE->text(), CREATE_A_TABLE->shortcut().toString()));
  EXPORT_TO_NO_ENCRYPTED_JSON = new QAction{QIcon{":/edit/EXPORT_TO_ENCRYPTED_JSON"}, "Export to json"};
  EXPORT_TO_IMAGE = new QAction{QIcon{":/edit/EXPORT_TO_IMAGE"}, "Export to images"};

  ROW_EDIT_AG = new QActionGroup{this};
  ROW_EDIT_AG->addAction(INSERT_A_ROW);
  ROW_EDIT_AG->addAction(INSERT_ROWS);
  ROW_EDIT_AG->addAction(APPEND_ROWS);
  ROW_EDIT_AG->addAction(DELETE_ROWS);
  ROW_EDIT_AG->addAction(CREATE_A_TABLE);
  ROW_EDIT_AG->addAction(EXPORT_TO_NO_ENCRYPTED_JSON);
  ROW_EDIT_AG->addAction(EXPORT_TO_IMAGE);

  SAVE_CHANGES = new QAction{QIcon{":/edit/SAVE_CHANGES"}, "Save changes"};
  SAVE_CHANGES->setShortcut(QKeySequence(Qt::ControlModifier | Qt::Key_S));
  SAVE_CHANGES->setToolTip(QString("<b>%1 (%2)</b><br/>Save Changes to local file."
                                   "Key16 must be correct.<br/>"
                                   "Otherwise json data will be corrupted")
                               .arg(SAVE_CHANGES->text(), SAVE_CHANGES->shortcut().toString()));
}

TableEditActions& GetTableEditActionsInst() {
  static TableEditActions ins;
  return ins;
}
