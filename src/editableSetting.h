#ifndef EDITABLESETTING_H
  #define EDITABLESETTING_H

  #include <QObject>
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
    editableSetting(QString settingName, QString settingLabel, QVariant settingData, QVariant min, QVariant max, QObject *parent=0);

    QWidget* label();
    QWidget* editor();
    QVariant data();

    void setLabelText(QString labelText);

    void readXML(QDomElement root);
    void writeXML(QIODevice *xml);

  private slots:
    void updateData();
    
  private:
    QString name;
    QString labelTxt;
    QVariant p_data;

    QLabel *labelWidget;
    QLabel *noEditLabel;
    QSpinBox *intSpinBox;
    QDoubleSpinBox *dblSpinBox;
};
#endif
