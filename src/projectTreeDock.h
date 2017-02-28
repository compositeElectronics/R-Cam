#ifndef PROJECTTREEDOCK_H
  #define PROJECTTREEDOCK_H

  #include <QDockWidget>
  #include <QTreeWidget>
  
class projectTreeDock : public QDockWidget
{
  Q_OBJECT
  public:
    projectTreeDock(QWidget *parent=0);

    QTreeWidget *treeWidget;
};
#endif
