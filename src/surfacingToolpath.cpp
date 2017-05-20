#include "surfacingToolpath.h"

surfacingToolpath::surfacingToolpath(rcamObject *parent) : genericToolpath(parent, "surfacingToolpath"){
  setting.append(new editableSetting(QString("zStep"), QString("Z Step"),   QVariant((double)-0.5), QVariant(-100), QVariant(-0.0001)));
  createSettingsTable();
  createMenu();
}

void surfacingToolpath::addGeometryByLayer(){
  int i;
  genericToolpath::addGeometryByLayer();
  for (i=0;i<children.count();i++){
    children[i]->setting.append(new editableSetting(QString("stepDown"), QString("Step Down"), QVariant((bool)true), QVariant(), QVariant()));
    children[i]->createSettingsTable();
  }
  
}

void surfacingToolpath::polishTreeLabel(){
  treeItem->setText(0,QString("Surfacing Toolpath"));  
}

void surfacingToolpath::calcToolPath(const ON_Surface* srf, geomReference* geomRef){
  int nSrfs, i;
  double u, v, umin, umax, vmin, vmax, uStep, vStep, toolRadius;
  char line[256];
  
  bool takeNextCut;
  double z, zStep, zi;
  bool ramp, stepDown;
  QVariant setting;
  
  ON_3dPoint srfPt, toolPt, toolDelta;
  ON_3dVector normal;

  srf->GetDomain(0, &umin, &umax);
  srf->GetDomain(1, &vmin, &vmax);
  
  toolRadius=3;
  toolDelta=ON_3dPoint(0, 0, -toolRadius);
  
  zStep=findSettingValue("zStep").toDouble(); 
  
  uStep=(umax-umin)/10.;
  vStep=(vmax-vmin)/10.;
  
  setting=geomRef->findSettingValue("stepU"); if (setting!=QVariant()) uStep=setting.toDouble();
  setting=geomRef->findSettingValue("stepV"); if (setting!=QVariant()) vStep=setting.toDouble();
  
  stepDown=geomRef->findSettingValue("stepDown").toBool();
  
  z=0;
  if (!stepDown) z=-1000;
  takeNextCut=true;
  while (takeNextCut){
    z+=zStep;
    takeNextCut=false;
//    printf("Z=%lf , uStep=%lf , vStep=%lf\n",z,uStep, vStep);
    for (u=umin; u<=umax+.001; u+=uStep){
      srfPt=srf->PointAt(u,vmin);
      normal=srf->NormalAt(u,vmin);
      toolPt=srfPt+normal*toolRadius+toolDelta;
      sprintf(line,"G0 X%lf Y%lf",toolPt.x, toolPt.y);
      path.append(line);
      path.append("G0 Z[#<workZ>+#<rapidZ>]");
      for (v=vmin; v<=vmax+.001; v+=vStep){

        srfPt=srf->PointAt(u,v);
        normal=srf->NormalAt(u,v);
      
        toolPt=srfPt+normal*toolRadius+toolDelta;
      
//        printf("     xyz = %lf %lf %lf\n", srfPt.x, srfPt.y, srfPt.z);
//        printf("  normal = %lf %lf %lf\n", normal.x, normal.y, normal.z);
//        printf("tool xyz = %lf %lf %lf\n", toolPt.x, toolPt.y, toolPt.z);
      
        sprintf(line,"G1 X%lf Y%lf",toolPt.x, toolPt.y);
        if (toolPt.z>z){
          sprintf(line,"%s Z[#<workZ>+%lf]", line, toolPt.z);
        }else{
          sprintf(line,"%s Z[#<workZ>+%lf]", line, z);
          takeNextCut=true;
//          printf("Must take next cut\n");
        }
        path.append(line);
      }
      path.append("G0 Z[#<workZ>+#<rapidZ>]");
    }
  }
}
