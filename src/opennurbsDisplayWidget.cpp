#include "opennurbsDisplayWidget.h"
#include "opennurbs_20130711/opennurbs.h"
#include <QPainter>

opennurbsDisplayWidget::opennurbsDisplayWidget(QWidget *parent) : QWidget(parent)
{
  model=0;
  scale=20.;
  
  xAxisPen.setColor(QColor(120,0,0));
  xAxisPen.setWidth(3);
  xAxisPen.setStyle(Qt::DashDotDotLine);
  
  yAxisPen.setColor(QColor(0,180,0));
  yAxisPen.setWidth(3);
  yAxisPen.setStyle(Qt::DashDotDotLine);
  
  gridPen.setColor(QColor(180,180,180));
  gridPen.setWidth(0);
  gridPen.setStyle(Qt::DotLine);
  
  selectColour=QColor(230,230,0);
  
  drawObject=-1; // By default draw all objects.
  hasModel=false;
  autoCenterAndZoom=false;
  offset=QPointF(0,0);
  setMinimumSize(100, 100);
}

void opennurbsDisplayWidget::setModelGeometry(ONX_Model *geometry){
  model=geometry;
  hasModel=true;
}

void opennurbsDisplayWidget::setObjectDrawn(int obj){
  drawObject=obj;
}

void opennurbsDisplayWidget::setSelectedObjects(QList<geomReference*> objs){
  selectedObjects.clear();
  selectedObjects.append(objs);
  update();
}

void opennurbsDisplayWidget::setSelectedObjects(geomReference* objs){
  selectedObjects.clear();
  selectedObjects.append(objs);
  update();
}

void opennurbsDisplayWidget::clearSelectedObjects(){
  selectedObjects.clear();
  update();
}

void opennurbsDisplayWidget::setAutoCenterAndZoom(bool autoCenter){
  autoCenterAndZoom=autoCenter;
}

void opennurbsDisplayWidget::centreAndZoom(){
  QPointF midWindow, midModel;
  double xScale, yScale;
  
  if (!hasModel) return;
  if (model->m_object_table.Count()==0) return;
  
  printf("Interrogating bounding box\n");
  ON_BoundingBox bounds=model->BoundingBox();
  printf("Complete\n");
  xScale=(double)width()/bounds.Diagonal().x;
  yScale=(double)height()/bounds.Diagonal().y;
  
  scale=xScale;
  if (yScale<xScale) scale=yScale;
  scale*=0.98;
  midWindow=QPointF(width()/2, height()/2);
  midModel=QPointF((bounds.Min().x+bounds.Max().x)/2., -(bounds.Min().y+bounds.Max().y)/2.);
  offset=midWindow-midModel*scale;
  
  printf("Model Bounds\n");
  printf("  X = %lf\t%lf\n",bounds.Min().x, bounds.Max().x);
  printf("  Y = %lf\t%lf\n",bounds.Min().y, bounds.Max().y);
  printf("  Z = %lf\t%lf\n",bounds.Min().z, bounds.Max().z);
  printf("Model Center\n");
  printf("  X = %lf\n",(bounds.Min().x+bounds.Max().x)/2.);
  printf("  Y = %lf\n",(bounds.Min().y+bounds.Max().y)/2.);
  printf("  Z = %lf\n",(bounds.Min().z+bounds.Max().z)/2.);
  
  printf("Offset = %lf        %lf\n",offset.x(), offset.y());
}

void opennurbsDisplayWidget::paintEvent(QPaintEvent *event){
  int objIndex=0; // maximum is m_object_table.Count()
  float x, y;
  QPointF p0, p1;  
  if (!model) return;
  
  if (autoCenterAndZoom) centreAndZoom();
  
  QPainter painter(this);
  
  painter.setPen(gridPen);
  // Draw Grid
  for (x=-100;x<100.1;x+=10*scale){
    p0=QPointF(x,-100)*scale+offset;
    p1=QPointF(x,100)*scale+offset;
    painter.drawLine(p0,p1);
  }
  
  for (y=-100;y<100.1;y+=10*scale){
    p0=QPointF(-100,y)*scale+offset;
    p1=QPointF(100,y)*scale+offset;
    painter.drawLine(p0,p1);
  }

  painter.setPen(xAxisPen);
  p0=QPointF(-10,0)*scale+offset;
  p1=QPointF(10,0)*scale+offset;
  painter.drawLine(p0,p1);

  painter.setPen(yAxisPen);
  p0=QPointF(0,-10)*scale+offset;
  p1=QPointF(0,10)*scale+offset;
  painter.drawLine(p0,p1);
    
  if (!hasModel) return;
  
  if (drawObject==-1){
    for (objIndex=0;objIndex<model->m_object_table.Count();objIndex++){
      drawModelObject(&painter, objIndex);
    }
  }else{
    if (drawObject>model->m_object_table.Count()) return;
    drawModelObject(&painter, drawObject);
  }
}

void opennurbsDisplayWidget::drawModelObject(QPainter *painter, int objIndex){
  const ON_Object* geom=0;
  ON_Color objColor;
  QPen modelPen;
  bool markers=false;
  bool reverse=false;
  int i;
  const ONX_Model_Object& mo = model->m_object_table[objIndex];
  
  if (ON::color_from_layer==mo.m_attributes.ColorSource())  objColor=model->m_layer_table[mo.m_attributes.m_layer_index].Color();
  if (ON::color_from_object==mo.m_attributes.ColorSource()) objColor=mo.m_attributes.m_color;
    
      // If ColorSource() is ON::color_from_layer, then the object's layer 
  // ON_Layer::Color() is used.
  // If ColorSource() is ON::color_from_object, then value of m_color is used.
  // If ColorSource() is ON::color_from_material, then the diffuse color of the object's
  // render material is used.  See ON_3dmObjectAttributes::MaterialSource() to
  // determine where to get the definition of the object's render material.
    
  modelPen.setColor(QColor(objColor.Red(), objColor.Green(), objColor.Blue()));
//  char *mref, *selref;
//  mref = new char[38];
//  selref = new char[38];
  markers=false;
  for (i=0;i<selectedObjects.count();i++){
//    mref=ON_UuidToString( mo.m_attributes.m_uuid , mref);
//    selref=ON_UuidToString( selectedObjects[i]->ref , selref);
//    printf("Checking ModelRef %s against Selected %s\n", mref, selref);
    if (mo.m_attributes.m_uuid==selectedObjects[i]->ref){
      modelPen.setColor(selectColour);
      markers=true;
      reverse=selectedObjects[i]->findSettingValue("reverse").toBool();
      break;
    }
  }
  
  modelPen.setWidth(2);
  modelPen.setStyle(Qt::SolidLine);
  painter->setPen(modelPen);
    
  geom=mo.m_object;
  switch(geom->ObjectType()){
    case ON::curve_object:
      drawCurveXY(painter, geom, markers, reverse);
//      printf("Object Index = %i\n", objIndex);
      break;
  }
}
    
void opennurbsDisplayWidget::drawCurveXY(QPainter *painter, const ON_Object* geom, bool markers, bool reverse){
  double tStart, tEnd;
  double t, tRange;
  ON_3dPoint onPt, nxtPt;
  QPointF pt0, pt;
 
  const ON_Curve* curve=ON_Curve::Cast(geom);
  
  if (!curve) return;
  
  curve->GetDomain(&tStart, &tEnd);
 
  for (t=tStart;t<=tEnd+.001;t+=0.1){
    onPt=curve->PointAt(t);
    pt=QPointF(onPt.x, onPt.y*-1.)*scale+offset;
    if (t>tStart+.001) painter->drawLine(pt,pt0);
    pt0=pt;
  }
  
  if (markers){
    tRange=tEnd-tStart;
    for (t=tStart;t<=tEnd;t+=tRange/10.){
      if (reverse){
        onPt=curve->PointAt(t+.01);
        nxtPt=curve->PointAt(t);
      }else{
        onPt=curve->PointAt(t);
        nxtPt=curve->PointAt(t+.01);
      }
      pt=QPointF(onPt.x, onPt.y*-1.)*scale+offset;
      if (nxtPt.x>onPt.x){
        painter->drawLine(pt.x(),pt.y()-5,pt.x()+5,pt.y());
        painter->drawLine(pt.x(),pt.y()+5,pt.x()+5,pt.y());
      }
      if (nxtPt.x<onPt.x){
        painter->drawLine(pt.x()+5,pt.y()-5,pt.x(),pt.y());
        painter->drawLine(pt.x()+5,pt.y()+5,pt.x(),pt.y());
      }
      
//      printf("Model Marker at %lf\t%lf\n",onPt.x, onPt.y);
    }
  }
}
