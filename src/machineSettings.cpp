#include "machineSettings.h"

machineSettings::machineSettings() : rcamObject(0,"machineSettings") {
  // Some default small machine settings...
  
  setting.append(new editableSetting(QString("xSize"),     QString("X Size"), QVariant((double)580), QVariant(0), QVariant(1000)));
  setting.append(new editableSetting(QString("ySize"),     QString("Y Size"), QVariant((double)400), QVariant(0), QVariant(1000)));
  setting.append(new editableSetting(QString("zSize"),     QString("Z Size"), QVariant((double)75), QVariant(0), QVariant(1000)));
  setting.append(new editableSetting(QString("maxFeed"),   QString("Max Feed"), QVariant((double)3000), QVariant(0), QVariant(1000)));
  setting.append(new editableSetting(QString("maxCutRPM"), QString("Max Cutter RPM"), QVariant((double)24000), QVariant(0), QVariant(1000)));
  setting.append(new editableSetting(QString("safeZ"),     QString("Safe Z"), QVariant((double)15), QVariant(0), QVariant(75)));
  setting.append(new editableSetting(QString("workZ"),     QString("Workpiece Z"), QVariant((double)0), QVariant(-75), QVariant(75)));
  setting.append(new editableSetting(QString("rapidZ"),    QString("Rapid Z above work"), QVariant((double)1), QVariant(0), QVariant(75)));
  createSettingsTable();
}

void machineSettings::polishTreeLabel(){
  treeItem->setFlags(treeItem->flags() & ~Qt::ItemIsEditable);
  treeItem->setText(0,QString("Machine Settings"));  
}

double machineSettings::xSize(){ return setting[0]->data().toDouble(); }
double machineSettings::ySize(){ return setting[1]->data().toDouble(); }    
double machineSettings::zSize(){ return setting[2]->data().toDouble(); }
    
double machineSettings::maxFeed(){ return setting[3]->data().toDouble(); }
double machineSettings::maxCutterRPM(){ return setting[4]->data().toDouble(); }

double machineSettings::safeZ(){ return setting[5]->data().toDouble(); }
double machineSettings::workpieceZ(){ return setting[6]->data().toDouble(); }
