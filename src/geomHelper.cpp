#include "geomHelper.h"
#include <QList>

bool geomHelper::ptIsInTrimmedAreaOfBREP(ON_Brep *brep, double u, double v, double quality){
  int i, nIsect;
  double t, tStart, tEnd, tStep, tl, th;
  bool stop;
  QList<int> trmCrvIdx;
  ON_3dPoint pt0, pt1;
  ON_Curve *trm;
  ON_BoundingBox bbox;
  
//  printf("Position (UV) %lf %lf\n", u, v);
  
  // Find trim curves which intersect this point
  for (i=0;i<brep->m_T.Count();i++){
    trm=(ON_Curve*)brep->m_T[i].TrimCurveOf();
    bbox=trm->BoundingBox();
    if (bbox.Min().y<v && bbox.Max().y>v){
      trmCrvIdx.append(i);
//      printf("Trim curve %i may intersect ray, adding to list\n",i);
    }
  }
  
  // Find intersection points of trim curves which may intersect...
  // Simplistically, take 50 points on a trim curve and count the number of intersections.
  nIsect=0;
  for (i=0;i<trmCrvIdx.count();i++){
//    printf("Trim Curve %i\n",trmCrvIdx[i]);
    trm=(ON_Curve*)brep->m_T[trmCrvIdx[i]].TrimCurveOf();
    trm->GetDomain(&tStart, &tEnd);
    tStep=(tEnd-tStart)/quality;
    pt0=trm->PointAt(tStart);
    stop=false;
    for (t=tStart+tStep;t<tEnd+tStep/2;t+=tStep){
      if (trm->IsClosed() && t>tEnd){
        t=tStart+(t-tEnd);
        stop=true;
      }
      pt1=trm->PointAt(t);
      if (((pt0.y<v && pt1.y>=v) || (pt1.y<=v && pt0.y>v)) && !(pt0.x<u && pt1.x<u)){
//        printf("Intersection found between t=%lf and t=%lf\n",t-tStep, t);
//        printf("Intersection found between U/V %lf %lf and %lf %lf\n", pt0.x, pt0.y, pt1.x, pt1.y);
        nIsect++;
      }
      pt0=pt1;
      if (stop) break;
    }
  }
//  printf("Intersections=%i\n",nIsect);
  if ((nIsect % 2)!=0) return true;
  return false;
}

double geomHelper::surfaceLengthU(const ON_Surface *srf){
  int i, stations=5;
  double u, v, uMin, uMax, vMin, vMax, uStep, vStep, length, maxLength;
  ON_3dPoint pt0, pt1, delta;
  
  srf->GetDomain(0, &uMin, &uMax);
  srf->GetDomain(1, &vMin, &vMax);

  // Calculate lengths
  // Average of 5 lengths in V
  uStep=(uMax-uMin)/100.;
  vStep=(vMax-vMin)/(double)stations;
  maxLength=0;
  for (v=vMin;v<vMax+vStep/2.;v+=vStep){
    length=0;
    pt0=srf->PointAt(u,v);
    for (u=uMin;u<uMax+uStep/2.;u+=uStep){
      pt1=srf->PointAt(u,v);
      delta=pt1-pt0;
      length+=sqrt(pow(delta.x,2)+pow(delta.y,2)+pow(delta.z,2));
      pt0=pt1;
    }
//    printf("U Length = %lf\n", length);
    if (length>maxLength) maxLength=length;
  }
//  printf("U Max Length = %lf\n", maxLength);
  return maxLength;
}

double geomHelper::surfaceLengthV(const ON_Surface *srf){
  int i, stations=5;
  double u, v, uMin, uMax, vMin, vMax, uStep, vStep, length, maxLength;
  ON_3dPoint pt0, pt1, delta;
  
  srf->GetDomain(0, &uMin, &uMax);
  srf->GetDomain(1, &vMin, &vMax);
  
  // Calculate lengths
  // Average of 5 lengths in V
  uStep=(uMax-uMin)/(double)stations;
  vStep=(vMax-vMin)/100.;
  maxLength=0;
  for (u=uMin;u<uMax+uStep/2.;u+=uStep){
    length=0;
    pt0=srf->PointAt(u,v);
    for (v=vMin;v<vMax+vStep/2.;v+=vStep){
      pt1=srf->PointAt(u,v);
      delta=pt1-pt0;
      length+=sqrt(pow(delta.x,2)+pow(delta.y,2)+pow(delta.z,2));
      pt0=pt1;
    }
//    printf("V Length = %lf\n", length);
    if (length>maxLength) maxLength=length;
  }
//  printf("V Max Length = %lf\n", maxLength);
      
  return maxLength;
}
