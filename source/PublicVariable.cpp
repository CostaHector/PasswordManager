#include "include/PublicVariable.h"

QString ViewStyleSheet::GetDefaultTableViewStyleSheet() {
  static const QString styleSheet =
      "QTableView {"
      "    show-decoration-selected: 1;"
      "}"
      "QTableView::item:alternate {"
      "}"
      "QTableView::item:selected {"
      "    border-bottom: 1px inherit #FFFFFF;"
      "}"
      "QTableView::item:selected:!active {"
      "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #EEEEEE, stop: 1 #999999);"
      "    color: #000000;"
      "}"
      "QTableView::item:selected:active {"
      "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #99D1FF, stop: 1 #99D1FF);"
      "    color: #000000;"
      "    border-top: 2px solid #CCEBFF;"
      "    border-bottom: 2px solid #CCEBFF;"
      "}"
      "QTableView::item:hover {"
      "    background: #CCEBFF;"
      "    color: #000000;"
      "}";
  return styleSheet;
}
