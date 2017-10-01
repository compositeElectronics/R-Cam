#include "geomHelper.h"
#include <QList>

bool geomHelper::ptIsInTrimmedAreaOfBREP(ON_Brep *brep, double u, double v){
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
    tStep=(tEnd-tStart)/200.;
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

