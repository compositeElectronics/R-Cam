#include "surfacingToolpath.h"

surfacingToolpath::surfacingToolpath(rcamObject *parent) : genericToolpath(parent, "surfacingToolpath"){
  setting.append(new editableSetting(QString("zStep"), QString("Z Step"),   QVariant((double)-0.5), QVariant(-100), QVariant(-0.0001)));
  setting.append(new editableSetting(QString("zFeed"), QString("Z Feed"),   QVariant((double)50), QVariant(1), QVariant(1000)));
  setting.append(new editableSetting(QString("toolRadius"), QString("Tool Radius"),   QVariant((double)3), QVariant(0), QVariant(100)));
  setting.append(new editableSetting(QString("srfOffset"), QString("Surface Offset"),   QVariant((double)0), QVariant(-100), QVariant(100)));
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
  
  bool takeNextCut, stepU, stepV;
  double z, zStep, zi, zFeed, srfOffset;
  bool ramp, stepDown, flipNormals, isFirst, isRapid;
  QVariant setting;
  
  ON_3dPoint srfPt, toolPt, toolDelta;
  ON_3dVector normal;

  srf->GetDomain(0, &umin, &umax);
  srf->GetDomain(1, &vmin, &vmax);
  
  toolRadius=findSettingValue("toolRadius").toDouble();
  srfOffset=findSettingValue("srfOffset").toDouble();
  toolDelta=ON_3dPoint(0, 0, -toolRadius);
  
  zStep=findSettingValue("zStep").toDouble(); 
  zFeed=findSettingValue("zFeed").toDouble();
  
  uStep=(umax-umin)/10.;
  vStep=(vmax-vmin)/10.;
  
  setting=geomRef->findSettingValue("stepU"); if (setting!=QVariant()) uStep=setting.toDouble();
  setting=geomRef->findSettingValue("stepV"); if (setting!=QVariant()) vStep=setting.toDouble();
  
  stepDown=geomRef->findSettingValue("stepDown").toBool();
  flipNormals=geomRef->findSettingValue("flipNormals").toBool();

  sprintf(line,"#<zFeed>=%lf",zFeed); path.append(line);
  path.append("F #<stdFeed>");
  
  z=findSettingValue("startZ").toDouble();
  if (!stepDown) z=-1000;
  takeNextCut=true;
  isRapid=false;
  while (takeNextCut){
    z+=zStep;
    takeNextCut=false;
    printf("uMin=%lf , uMax=%lf\n", umin, umax);
    printf("vMin=%lf , vMax=%lf\n", vmin, vmax);
    printf("Z=%lf , uStep=%lf , vStep=%lf\n",z,uStep, vStep);
    u=umin;
    stepU=true;
    while (stepU){
      if (u>umax){ u=umax; stepU=false; }
      
      isFirst=true;
      v=vmin;
      stepV=true;
      while (stepV){
        if (v>vmax){ v=vmax; stepV=false; }

        srfPt=srf->PointAt(u,v);
        normal=srf->NormalAt(u,v);
      
        if (srf->IsAtSingularity(u,v,false)){
          printf("Ignoring surface point %lf,%lf - point is a singularity\n",u,v);
          v+=vStep;
          continue;
        }
    
        if (!flipNormals){
          toolPt=srfPt+normal*(toolRadius+srfOffset);
        }else{
          toolPt=srfPt-normal*(toolRadius+srfOffset);
        }
      
        toolPt+=toolDelta;
      
//        printf("     xyz = %lf %lf %lf\n", srfPt.x, srfPt.y, srfPt.z);
//        printf("  normal = %lf %lf %lf\n", normal.x, normal.y, normal.z);
//        printf("tool xyz = %lf %lf %lf\n", toolPt.x, toolPt.y, toolPt.z);
      
        
        if (stepDown && toolPt.z>z+zStep){
          if (!isRapid){
            printf("into rapid - u=%lf\n",u);
            path.append("G0 Z[#<workZ>+#<rapidZ>]");
            isRapid=true;
          }
          v+=vStep;
          continue;
        }
        
        if (isRapid){
          printf("out of rapid - u=%lf\n",u);
          sprintf(line,"G0 X%lf Y%lf",toolPt.x, toolPt.y);
          path.append(line);
          isFirst=true;
        }
        isRapid=false;
        
        sprintf(line,"G1 X%lf Y%lf",toolPt.x, toolPt.y);
        if (toolPt.z>z){
          sprintf(line,"%s Z[#<workZ>+%lf]", line, toolPt.z);
        }else{
          sprintf(line,"%s Z[#<workZ>+%lf]", line, z);
          takeNextCut=true;
//          printf("Must take next cut\n");
        }
        
        if (isFirst){
          sprintf(line,"G0 X%lf Y%lf",toolPt.x, toolPt.y);
          path.append(line);
          if (toolPt.z>z){
            sprintf(line,"G0 Z[#<workZ>+%lf]", line, toolPt.z-zStep);
          }else{
            sprintf(line,"G0 Z[#<workZ>+%lf]", line, z-zStep);
          }
          path.append(line);
          path.append("F #<zFeed>");
          if (toolPt.z>z){
            sprintf(line,"G1 Z[#<workZ>+%lf]", line, toolPt.z);
          }else{
            sprintf(line,"G1 Z[#<workZ>+%lf]", line, z);
          }
          path.append(line);
          path.append("F #<stdFeed>");
          isFirst=false;
        }else{
          path.append(line);
        }
        v+=vStep;
      }
      path.append("G0 Z[#<workZ>+#<rapidZ>]");
      u+=uStep;
    }
  }
}
