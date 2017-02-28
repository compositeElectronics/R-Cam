#include "settingsDock.h"

settingsDock::settingsDock(QWidget *parent) : QDockWidget(QString("Object Settings"), parent){
  setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  
  blankSettingsTable = new QTableWidget(1,2);
    
  setWidget(blankSettingsTable);
}

void settingsDock::setRcamObject(rcamObject *obj){
  setWidget(obj->settingsTable());
}
