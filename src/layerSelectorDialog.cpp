#include "layerSelectionDialog.h"

layerSelectionDialog::layerSelectionDialog(QWidget *parent) : QDialog(parent){
  layerList = new QListWidget(this);
  
  new QListWidgetItem(tr("Oak"), layerList);
  
}

void layerSelectionDialog::setOpenNurbsModel(const ONX_Model* modelGeom){
  model=modelGeom;
}
