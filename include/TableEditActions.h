#ifndef TABLEEDITACTIONS_H
#define TABLEEDITACTIONS_H
#include <QObject>
#include <QAction>
#include <QActionGroup>

class TableEditActions : public QObject {
 public:
  explicit TableEditActions(QObject* parent = nullptr);

  QAction *INSERT_A_ROW{nullptr}, *INSERT_ROWS{nullptr}, *APPEND_ROWS{nullptr};
  QAction *DELETE_ROWS{nullptr};
  QAction *EXPORT_TO_PLAIN_CSV{nullptr};
  QActionGroup* ROW_EDIT_AG{nullptr};
  QAction* SAVE_CHANGES{nullptr};
};

TableEditActions& GetTableEditActionsInst();

#endif  // TABLEEDITACTIONS_H
