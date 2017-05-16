#ifndef EDITABLESETTING_H
  #define EDITABLESETTING_H

  #include <QObject>
  #include <QCheckBox>
  #include <QSpinBox>
  #include <QDoubleSpinBox>
  #include <QLabel>
  #include <QString>
  #include <QList>
  #include <QTreeWidget>  
  #include <QMenu>
  #include <QIODevice>
  #include <QDomDocument>

class editableSetting : public QObject
{
  Q_OBJECT
  public:
    editableSetting(QDomElement element, QObject *parent=0);
    editableSetting(QString settingName, QString settingLabel, QVariant settingData, QVariant min, QVariant max, QObject *parent=0);

    QString settingName();
    QWidget* label();
    QWidget* editor();
    QVariant data();

    void setLabelText(QString labelText);

    void readXMLElement(QDomElement element);
    void writeXML(QIODevice *xml, int xmlLevel=0);

    bool isApplicable(QDomElement xml);
    
  private slots:
    void updateData();
    
  private:
    QString name;
    QString labelTxt;
    QVariant p_data;

    QLabel *labelWidget;
    QLabel *noEditLabel;
    QCheckBox *checkBox;
    QSpinBox *intSpinBox;
    QDoubleSpinBox *dblSpinBox;

    void createSetting(QString settingName, QString settingLabel, QVariant settingData, QVariant min, QVariant max);
};
#endif
