#ifndef OPENNURBSDISPLAYWIDGET_H
  #define OPENNURBSDISPLAYWIDGET_H

  #include <QWidget>
  #include <QPen>
  #include "opennurbs_20130711/opennurbs.h"
  
class opennurbsDisplayWidget : public QWidget
{
  Q_OBJECT
  public:
    opennurbsDisplayWidget(QWidget *parent=0);
    void setModelGeometry(ONX_Model *geometry);
    void setModelGeometry(const ONX_Model_Object& mo);
    void centreAndZoom();
    
    void setObjectDrawn(int obj);
    void setObjectSelected(int obj);
    void setShowMarkers(bool show);
    void setAutoCenterAndZoom(bool autoCenter);
    
  private:
    int drawObject;
    int selectedObject;
    bool hasModel;
    bool showMarkers;
    bool autoCenterAndZoom;
    ONX_Model *model;
    double scale;
    
    void paintEvent(QPaintEvent *event);
    void drawModelObject(QPainter *painter, int objIndex);
    void drawCurveXY(QPainter *painter, const ON_Object* geom=0);
    
    QPen xAxisPen;
    QPen yAxisPen;
    QPen gridPen;
    QColor selectColour;
    QPointF offset;
};
#endif
