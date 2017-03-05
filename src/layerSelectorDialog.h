#ifndef LAYERSELECTIONDIALOG_H
  #define LAYERSELECTIONDIALOG_H

  #include <QDialog>
  #include <QListWidget>
  
class layerSelectionDialog : public QDialog
{
  Q_OBJECT
  public:
    layerSelectionDialog(QWidget *parent=0);
    void setOpenNurbsModel(const ONX_Model* modelGeom);

  private:
    QListWidget *layerList;
    const ONX_Model* model;
};
#endif
