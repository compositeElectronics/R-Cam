#ifndef MACHINESETTINGS_H
  #define MACHINESETTINGS_H

  #include <QDialog>
  #include <QMenu>
  #include <QMenuBar>
  #include <QAction>
  #include <QDoubleSpinBox>
  #include "rcamObject.h"
  
class machineSettings : public rcamObject
{
  Q_OBJECT
  public:
    machineSettings();

    double xSize();
    double ySize();
    double zSize();
    
    double safeZ();
    double workpieceZ();
    
    double maxFeed();
    double maxCutterRPM();
    
  private:
    QList<QTreeWidgetItem*> settingsTreeItem;
};
#endif
