#ifndef OPENNURBSDISPLAYWIDGET_H
  #define OPENNURBSDISPLAYWIDGET_H

  #include <QWidget>
  #include <QPen>
  #include "opennurbs_20130711/opennurbs.h"
  #include "geomReference.h"

class opennurbsDisplayWidget : public QWidget
{
  Q_OBJECT
  public:
    opennurbsDisplayWidget(QWidget *parent=0);
    void setModelGeometry(ONX_Model *geometry);
    void setModelGeometry(const ONX_Model_Object& mo);
    void centreAndZoom();
    
    void setObjectDrawn(int obj);
    void setSelectedObjects(QList<geomReference*> objs);
    void setSelectedObjects(geomReference* objs);
    void clearSelectedObjects();
    void setAutoCenterAndZoom(bool autoCenter);
    
  private:
    int drawObject;
    QList<geomReference*> selectedObjects;
    bool hasModel;
    bool autoCenterAndZoom;
    ONX_Model *model;
    double scale;
    
    void paintEvent(QPaintEvent *event);
    void drawModelObject(QPainter *painter, int objIndex);
    void drawCurveXY(QPainter *painter, const ON_Object* geom, bool markers, bool reverse, double step);
    
    QPen xAxisPen;
    QPen yAxisPen;
    QPen gridPen;
    QColor selectColour;
    QPointF offset;
};
#endif
