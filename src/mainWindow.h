#ifndef MAINWINDOW_H
  #define MAINWINDOW_H

  #include <QMainWindow>
  #include <QSplitter>
  #include <QTreeWidget>
  #include <QMenu>
  #include <QMenuBar>
  #include <QAction>
  #include <QScrollArea>
  
  #include "opennurbs_20130711/opennurbs.h"
  #include "opennurbsDisplayWidget.h"
  
  #include "genericToolpath.h"
  #include "machineSettings.h"
  #include "projectTreeDock.h"
  #include "settingsDock.h"
  
class mainWindow : public QMainWindow
{
  Q_OBJECT
  public:
    mainWindow(QMainWindow *parent=0);

  private:
    QMenu *fileMenu;
    QAction *openGeometryAction;
    QAction *saveGCodeAction;
    
    QMenu *operationsMenu;
    QAction *createEngravePathAction;
    QAction *createProfilingPathAction;
    QAction *calculateToolPathAction;
  
    machineSettings *machine;
    ONX_Model model;
    
    QList<genericToolpath*> toolPath;
    
    projectTreeDock *projectTree;
    settingsDock *objectSettings;
    opennurbsDisplayWidget *geomDisplay;
  
  private slots:
    void openGeometry(QString fileName=QString());
    void saveGCode(QString fileName=QString());

    void engraveObject();
    void profileObject();

    void calculateToolpaths();
    
    void treeWidgetContextMenu(QPoint pt);
    void treeItemActivated(QTreeWidgetItem *item, int column);
};
#endif
