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
  
//  printf("Interrogating bounding box\n");
  ON_BoundingBox bounds=model->BoundingBox();
//  printf("Complete\n");
  xScale=(double)width()/bounds.Diagonal().x;
  yScale=(double)height()/bounds.Diagonal().y;
  
  scale=xScale;
  if (yScale<xScale) scale=yScale;
  scale*=0.95;
  midWindow=QPointF(width()/2, height()/2);
  midModel=QPointF((bounds.Min().x+bounds.Max().x)/2., -(bounds.Min().y+bounds.Max().y)/2.);
  offset=midWindow-midModel*scale;
/*  
  printf("Model Bounds\n");
  printf("  X = %lf\t%lf\n",bounds.Min().x, bounds.Max().x);
  printf("  Y = %lf\t%lf\n",bounds.Min().y, bounds.Max().y);
  printf("  Z = %lf\t%lf\n",bounds.Min().z, bounds.Max().z);
  printf("Model Center\n");
  printf("  X = %lf\n",(bounds.Min().x+bounds.Max().x)/2.);
  printf("  Y = %lf\n",(bounds.Min().y+bounds.Max().y)/2.);
  printf("  Z = %lf\n",(bounds.Min().z+bounds.Max().z)/2.);
  
  printf("Offset = %lf        %lf\n",offset.x(), offset.y());
  */
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
  double step=-1, ustep=-1, vstep=-1;
  QVariant setting;
  
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
      step=selectedObjects[i]->findSettingValue("step").toDouble();
   
      setting=selectedObjects[i]->findSettingValue("stepU"); if (setting!=QVariant()) ustep=setting.toDouble();
      setting=selectedObjects[i]->findSettingValue("stepV"); if (setting!=QVariant()) vstep=setting.toDouble();
      break;
    }
  }
  
  modelPen.setWidth(2);
  modelPen.setStyle(Qt::SolidLine);
  painter->setPen(modelPen);
    
  geom=mo.m_object;
  switch(geom->ObjectType()){
    case ON::curve_object:
      drawCurveXY(painter, geom, markers, reverse, step);
      break;
    case ON::brep_object:
      modelPen.setWidth(0);
      modelPen.setStyle(Qt::SolidLine);
      painter->setPen(modelPen);
      drawBrepXY(painter, geom, ustep, vstep);
      break;
  }
}
    
void opennurbsDisplayWidget::drawCurveXY(QPainter *painter, const ON_Object* geom, bool markers, bool reverse, double step){
  double tStart, tEnd;
  double t, tRange;
  ON_3dPoint onPt, nxtPt;
  QPointF pt0, pt;
 
  const ON_Curve* curve=ON_Curve::Cast(geom);
  
  if (!curve) return;
  
  curve->GetDomain(&tStart, &tEnd);
 
  if (step<1e-9) step=0.1;
  
  for (t=tStart;t<=tEnd+.001;t+=step){
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

void opennurbsDisplayWidget::drawBrepXY(QPainter *painter, const ON_Object* geom, double ustep, double vstep){
  bool stepU, stepV;
  int nSrfs, i, j;
  double u, v, umin, umax, vmin, vmax;
  const ON_Brep *brep=ON_Brep::Cast(geom);
  ON_Curve *trm;
  ON_Surface *srf;
  ON_MeshFaceRef meshFace;
  ON_3dPoint onPt;
  QPointF pt0, pt;
  QPen stdPen, modPen;
  
  if (!brep) return;
  /*
  nSrfs=brep->m_S.Count();
  printf("Object is a BREP with %i surfaces\n", nSrfs);
  printf("Object is a BREP with %i faces\n", brep->m_F.Count());
  printf("BREP has %i edge curves\n", brep->m_E.Count());
  printf("BREP has %i trim curves\n", brep->m_T.Count());
  printf("BREP has %i trim loops\n", brep->m_L.Count());
  */
  stdPen=painter->pen();
  
  for (i=0;i<brep->m_F.Count();i++){
    srf=(ON_Surface*)brep->m_F[i].SurfaceOf();
//    printf("Face %i...\n",i);
    
//    srf->GetDomain(0, &umin, &umax);
//    srf->GetDomain(1, &vmin, &vmax);
//    u=vmin+(umax-umin)/50; v=vmin+(vmax-vmin)/50;
//    ptIsInTrimmedAreaOfBREP((ON_Brep*)brep, u, v);
//    onPt=srf->PointAt(u,v); pt=QPointF(onPt.x, onPt.y*-1.)*scale+offset;
//    painter->drawEllipse(pt,5,5);
    
    // Draw render mesh - the bit we expect to be valid...
//    painter->setPen(QColor(0,0,255));
//    drawMeshXY(painter, brep->m_F[i].Mesh(ON::render_mesh));
    modPen=stdPen; modPen.setWidth(2); painter->setPen(modPen);
    for (j=0;j<brep->m_T.Count();j++){
      trm=(ON_Curve*)brep->m_T[j].TrimCurveOf();      
      drawBrepTrimCurveXY(painter, trm, srf);
    }
    painter->setPen(stdPen);
    
    srf->GetDomain(0, &umin, &umax);
    srf->GetDomain(1, &vmin, &vmax);
//    printf("Umin/Umax = %lf / %lf\n",umin,umax);
//    printf("Vmin/Vmax = %lf / %lf\n",vmin,vmax);
    
    if (ustep<1e-9) ustep=(umax-umin)/4.;
    if (vstep<1e-9) vstep=(vmax-vmin)/4.;

    u=umin;
    stepU=true;
    while (stepU){
      if (u>umax){ u=umax; stepU=false; }
      
      onPt=srf->PointAt(u,vmin);
      pt0=QPointF(onPt.x, onPt.y*-1.)*scale+offset;
      stepV=true;
      v=vmin;
      while (stepV){
        if (v>vmax){ v=vmax; stepV=false; }
        onPt=srf->PointAt(u,v);
        pt=QPointF(onPt.x, onPt.y*-1.)*scale+offset;
        if (ptIsInTrimmedAreaOfBREP((ON_Brep*)brep, u, v)) painter->drawLine(pt0,pt);
        pt0=pt;
        v+=vstep/12.;
      }
      u+=ustep;
    }

    v=vmin;
    stepV=true;
    while (stepV){
      if (v>vmax){ v=vmax; stepV=false; }
        
      onPt=srf->PointAt(umin,v);
      pt0=QPointF(onPt.x, onPt.y*-1.)*scale+offset;
      
      u=umin;
      stepU=true;
      while (stepU){
        if (u>umax){ u=umax; stepU=false; }
        onPt=srf->PointAt(u,v);
        pt=QPointF(onPt.x, onPt.y*-1.)*scale+offset;
        if (ptIsInTrimmedAreaOfBREP((ON_Brep*)brep, u, v)) painter->drawLine(pt0,pt);
        pt0=pt;
        u+=ustep/12.;
      }
      v+=vstep;
    }
    
    // Label U
    v=vmin+(vmax-vmin)/5.;
    u=umin+(umax-umin)/5.;
    
    onPt=srf->PointAt(u,v);
    pt0=QPointF(onPt.x, onPt.y*-1.)*scale+offset;
    
    u=umin+2.*(umax-umin)/5.;
    onPt=srf->PointAt(u,v);
    pt=QPointF(onPt.x, onPt.y*-1.)*scale+offset;
    painter->drawLine(pt0,pt);
    painter->drawText(pt+QPointF(2,0), "U");
    
    v=vmin+1.05*(vmax-vmin)/5.;
    u=umin+1.9*(umax-umin)/5.;
    onPt=srf->PointAt(u,v);
    pt0=QPointF(onPt.x, onPt.y*-1.)*scale+offset;
    painter->drawLine(pt,pt0);


/*    
    painter->setPen(QColor(0,125,0));    
    // Mark region of surface...
    for (u=umin;u<umax;u+=(umax-umin)/100){
      for (v=vmin;v<vmax;v+=(vmax-vmin)/100){
        if (ptIsInTrimmedAreaOfBREP((ON_Brep*)brep, u, v)){
          onPt=srf->PointAt(u,v);
          pt=QPointF(onPt.x, onPt.y*-1.)*scale+offset;
          painter->drawPoint(pt);
        }
      }
    }
*/   
  }
}

void opennurbsDisplayWidget::drawBrepTrimCurveXY(QPainter *painter, ON_Curve *trm, ON_Surface *srf){
  double tStart, tEnd, step;
  double t, tRange;
  double u, v;
  ON_3dPoint onPt, nxtPt;
  QPointF pt0, pt;
    
  trm->GetDomain(&tStart, &tEnd);
  tRange=tEnd-tStart;
  step=tRange/100;
  
  for (t=tStart;t<=tEnd+.001;t+=step){
    onPt=trm->PointAt(t);
    onPt=srf->PointAt(onPt.x, onPt.y);
    pt=QPointF(onPt.x, onPt.y*-1.)*scale+offset;
    if (t>tStart+.001) painter->drawLine(pt,pt0);
    pt0=pt;
  }
}

void opennurbsDisplayWidget::drawMeshXY(QPainter *painter, const ON_Object* geom){
  int j, k, vn;
  ON_3dPoint onPt;
  QPointF pt0, pt;
  const ON_Mesh* msh=(ON_Mesh*)geom;
  
  if (!msh) return;
  
  for (j=0;j<msh->m_F.Count();j++){
    vn=4;
    if (msh->m_F[j].IsTriangle()) vn=3;
    onPt=msh->Vertex(msh->m_F[j].vi[0]);
    pt0=QPointF(onPt.x, onPt.y*-1.)*scale+offset;
    for (k=1;k<vn;k++){
      onPt=msh->Vertex(msh->m_F[j].vi[k]);
      pt=QPointF(onPt.x, onPt.y*-1.)*scale+offset;
      painter->drawLine(pt0,pt);
      pt0=pt;
    }
  }
}
