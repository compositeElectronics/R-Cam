#ifndef SETTINGSDOCK_H
  #define SETTINGSDOCK_H

  #include <QDockWidget>
  #include <QTableWidget>
  #include "rcamObject.h"
  
class settingsDock : public QDockWidget
{
  Q_OBJECT
  public:
    settingsDock(QWidget *parent=0);
    void setRcamObject(rcamObject *obj);

  private:
    QTableWidget *blankSettingsTable;
};
#endif
