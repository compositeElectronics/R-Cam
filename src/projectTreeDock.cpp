#include "projectTreeDock.h"

projectTreeDock::projectTreeDock(QWidget *parent) : QDockWidget(QString("Project Tree"), parent){
  setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  
  treeWidget = new QTreeWidget();
  treeWidget->setColumnCount(1);
  treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
  
  setWidget(treeWidget);
  
  if (parent!=0){
    connect(treeWidget, SIGNAL(customContextMenuRequested(QPoint)), parent, SLOT(treeWidgetContextMenu(QPoint)));
    connect(treeWidget, SIGNAL(itemActivated(QTreeWidgetItem*, int)), parent, SLOT(treeItemActivated(QTreeWidgetItem*, int)));
  }
}
