#ifndef RCAMOBJECT_H
  #define RCAMOBJECT_H

  #include <QObject>
  #include <QString>
  #include <QList>
  #include <QTreeWidget>
  #include <QMenu>
  #include <QIODevice>
  #include <QDomDocument>
  #include <QTableWidget>
  #include "editableSetting.h"

class rcamObject : public QObject
{
  Q_OBJECT
  public:
    rcamObject(rcamObject *parentObj, QString objType);
    QString type();
    void addItemToTree(QTreeWidget *parentTree);
    
    QMenu* menu();
    QWidget* settingsTable();

    void readXML(QDomElement root);
    void writeXML(QIODevice *xml);
    QVariant findSettingValue(QString name);

  public slots:
    void executeTree();
    virtual void execute();
    
  protected:
    unsigned int hlevel;
    rcamObject *p_parent;
    QList<rcamObject*> children;
    
    QString p_type;
    QTreeWidgetItem *treeItem;
    QTableWidget *p_settingsTable;
    
    QMenu *treeItemMenu;
    QMenu *objectMenu;
    
    QAction *executeAction;
    QAction *executeTreeAction;
    QList<QAction*> objectAction;
    
    QList<editableSetting*> setting;
    
    void createSettingsTable();
    
    void addItemToTree(QTreeWidgetItem *parentTreeItem);
    virtual void createMenu();
    void spaceXML(QIODevice *xml, bool inside=true);
    virtual void writeSettingsXML(QIODevice *xml);
    virtual void readXMLElement(QDomElement element);
    virtual void polishTreeLabel();
};
#endif
