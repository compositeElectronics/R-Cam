#include "editableSetting.h"

editableSetting::editableSetting(QDomElement element, QObject *parent) : QObject (parent){
  bool tmpBool=false;
  int tmpInt=0;
  double tmpDbl=0;
  QVariant tmpVar;
  
  if (element.attribute("type").toLower()=="bool")   tmpVar=QVariant(tmpBool);
  if (element.attribute("type").toLower()=="int")    tmpVar=QVariant(tmpInt);
  if (element.attribute("type").toLower()=="double") tmpVar=QVariant(tmpDbl);

  createSetting(element.attribute("name"), element.attribute("label"), tmpVar, tmpVar, tmpVar);
    
  readXMLElement(element);
}

editableSetting::editableSetting(QString settingName, QString settingLabel, QVariant settingData, QVariant min, QVariant max, QObject *parent) : QObject (parent) {
  createSetting(settingName, settingLabel, settingData, min, max);  
}

void editableSetting::createSetting(QString settingName, QString settingLabel, QVariant settingData, QVariant min, QVariant max){
  name=settingName;
  labelTxt=settingLabel;
  p_data=settingData;

  noEditLabel = new QLabel("No editor defined");
  labelWidget = new QLabel(labelTxt);

  switch(p_data.type()){
    case QVariant::Bool :
      checkBox = new QCheckBox();
      checkBox->setChecked(p_data.toBool());
      connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(updateData()));
      break;
    case QVariant::Int :
      intSpinBox = new QSpinBox();
      intSpinBox->setMinimum(min.toInt());
      intSpinBox->setMaximum(max.toInt());
      intSpinBox->setValue(p_data.toInt());
      connect(intSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateData()));
      updateData();
      break;
    case QVariant::Double :
      dblSpinBox = new QDoubleSpinBox();
      dblSpinBox->setMinimum(min.toDouble());
      dblSpinBox->setMaximum(max.toDouble());
      dblSpinBox->setValue(p_data.toDouble());
      connect(dblSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateData()));
      break;
  }
}

QString editableSetting::settingName(){
  return name;
}

QWidget* editableSetting::label(){
  return labelWidget;
}

QWidget* editableSetting::editor(){
  switch(p_data.type()){
    case QVariant::Bool :
      return checkBox;
      break;
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

bool editableSetting::isApplicable(QDomElement xml){
  if (xml.attribute("name").toLower()==name.toLower()) return true;
  return false;
}
    
void editableSetting::readXMLElement(QDomElement element){

  // Check that XML stanza is for settings (just because we're here it doesn't infer
  // that the data is useful)
  if (element.tagName().toLower()!="setting") return;

  // Now check that it applies to this setting (by name)
  if (!isApplicable(element)) return;

  printf("      XML name = %s\n",element.attribute("name").toLatin1().data());
  printf("      XML type = %s\n",element.attribute("type").toLatin1().data());

  if (element.attribute("type").toLower()=="bool") checkBox->setChecked(element.attribute("value").toLower()=="true");

  if (element.attribute("type").toLower()=="int"){
    intSpinBox->setMinimum(element.attribute("min").toInt());
    intSpinBox->setMaximum(element.attribute("max").toInt());
    intSpinBox->setValue(element.attribute("value").toInt());
  }

  if (element.attribute("type").toLower()=="double"){
    dblSpinBox->setMinimum(element.attribute("min").toDouble());
    dblSpinBox->setMaximum(element.attribute("max").toDouble());
    dblSpinBox->setValue(element.attribute("value").toDouble());
  }
  
  updateData();
}

void editableSetting::writeXML(QIODevice *xml, int xmlLevel){
  char line[4096];
  QByteArray sp = QByteArray("  ").repeated(xmlLevel);
  
  xml->write(sp);
  xml->write("<setting ");
  xml->write(QString("name=\""+name+"\" ").toLatin1());
  xml->write(QString("label=\""+labelTxt+"\" ").toLatin1());
  switch(p_data.type()){
    case QVariant::Bool :
      xml->write("type=\"bool\" value=\"");
      if (p_data.toBool()){
        xml->write("true");
      }else{
        xml->write("false");
      }
      xml->write("\"");
      break;
    case QVariant::Int :
      sprintf(line,"type=\"int\" min=\"%i\" max=\"%i\" value=\"%i\"",intSpinBox->minimum(), intSpinBox->maximum(), p_data.toInt()); xml->write(line);
      break;
    case QVariant::Double :
      sprintf(line,"type=\"double\" min=\"%lg\" max=\"%lg\" value=\"%lg\"",dblSpinBox->minimum(), dblSpinBox->maximum(), p_data.toDouble()); xml->write(line);
      break;
  }
  xml->write("/>\n");
}

void editableSetting::updateData(){
    switch(p_data.type()){
    case QVariant::Bool :
      p_data=checkBox->isChecked();
      break;      
    case QVariant::Int :
      p_data=intSpinBox->value();
      break;
    case QVariant::Double :
      p_data=dblSpinBox->value();
      break;
  }
}
