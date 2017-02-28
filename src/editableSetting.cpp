#include "editableSetting.h"

editableSetting::editableSetting(QString settingName, QString settingLabel, QVariant settingData, QVariant min, QVariant max, QObject *parent) : QObject (parent) {

  name=settingName;
  labelTxt=settingLabel;
  p_data=settingData;

  noEditLabel = new QLabel("No editor defined");
  labelWidget = new QLabel(labelTxt);

  switch(p_data.type()){
    case QVariant::Int :
      intSpinBox = new QSpinBox();
      intSpinBox->setMinimum(min.toInt());
      intSpinBox->setMaximum(max.toInt());
      intSpinBox->setValue(p_data.toInt());
      break;
    case QVariant::Double :
      dblSpinBox = new QDoubleSpinBox();
      dblSpinBox->setMinimum(min.toInt());
      dblSpinBox->setMaximum(max.toInt());
      dblSpinBox->setValue(p_data.toDouble());
      break;
  }
}

QWidget* editableSetting::label(){
  return labelWidget;
}

QWidget* editableSetting::editor(){
  switch(p_data.type()){
    case QVariant::Int :
      return intSpinBox;
      break;
    case QVariant::Double :
      return dblSpinBox;
      break;
  }
  
  return noEditLabel;
}

QVariant editableSetting::data(){
  return p_data;
}

void editableSetting::readXML(QDomElement root){
}

void editableSetting::writeXML(QIODevice *xml){
}

void editableSetting::updateData(){
    switch(p_data.type()){
    case QVariant::Int :
      p_data=intSpinBox->value();
      break;
    case QVariant::Double :
      p_data=dblSpinBox->value();
      break;
  }
}
