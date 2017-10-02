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

void surfacingToolpath::calcToolPath(const ON_Brep* brep, geomReference* geomRef, int srfIdx){
  int nSrfs, i;
  double u, v, uMin, uMax, vMin, vMax, uStep, vStep, toolRadius, r, s, rStp, sStp;
  double t, tMin, tMax, tStep;
  char line[256];
  
  bool takeNextCut, stepOver, stepAlong, domU;
  double z, zStep, zi, zFeed, srfOffset;
  bool ramp, stepDown, flipNormals, isFirst, isRapid;
  QVariant setting;
  
  ON_3dPoint onPt, toolPt;
  ON_3dVector normal;
  const ON_Surface *srf=brep->m_S[srfIdx];
  ON_Curve *trm;
  
  srf->GetDomain(0, &uMin, &uMax);
  srf->GetDomain(1, &vMin, &vMax);
  
  toolRadius=findSettingValue("toolRadius").toDouble();
  srfOffset=findSettingValue("srfOffset").toDouble();
  toolRadius+=srfOffset;
  
  zStep=findSettingValue("zStep").toDouble(); 
  zFeed=findSettingValue("zFeed").toDouble();
  
  stepOver=1;
  stepAlong=0.5;
  
  setting=geomRef->findSettingValue("stepAlong"); if (setting!=QVariant()) stepAlong=setting.toDouble();
  setting=geomRef->findSettingValue("stepOver"); if (setting!=QVariant()) stepOver=setting.toDouble();
  domU=geomRef->findSettingValue("domU").toBool();
  stepDown=geomRef->findSettingValue("stepDown").toBool();
  flipNormals=geomRef->findSettingValue("flipNormals").toBool();
  
  printf("Length in U: %lf\n", surfaceLengthU(srf));
  printf("Length in V: %lf\n", surfaceLengthV(srf));
  printf("Step Along Distance: %lf\n", stepAlong);
  printf("Step Over Distance: %lf\n", stepOver);

  if (domU){
    printf("Direction U is dominant\n");
    rStp=stepOver/surfaceLengthV(srf);
    sStp=stepAlong/surfaceLengthU(srf);
  }else{
    printf("Direction V is dominant\n");
    rStp=stepOver/surfaceLengthU(srf);
    sStp=stepAlong/surfaceLengthV(srf);
  }
  
  printf("RSTEP: %lf\n",rStp);
  printf("SSTEP: %lf\n",sStp);
  
  sprintf(line,"#<zFeed>=%lf",zFeed); path.append(line);
  path.append("F #<stdFeed>");
  
  z=findSettingValue("startZ").toDouble();
  if (!stepDown) z=-1000;

  takeNextCut=true;
  isRapid=false;
  while (takeNextCut){
    z+=zStep;
    takeNextCut=false;
//    printf("uMin=%lf , uMax=%lf\n", umin, umax);
//    printf("vMin=%lf , vMax=%lf\n", vmin, vmax);
//    printf("Z=%lf , uStep=%lf , vStep=%lf\n",z,uStep, vStep);
    
    printf("Taking cut limited at Z=%lf\n", z);
    for (r=0;r<1+rStp/2;r+=rStp){
      isFirst=true;
      for (s=0;s<1+sStp/2;s+=sStp){
        if (domU){
          u=uMin+s*(uMax-uMin);
          v=vMin+r*(vMax-vMin);
        }else{
          u=uMin+r*(uMax-uMin);
          v=vMin+s*(vMax-vMin);
        }

        if (srf->IsAtSingularity(u,v,false) || !ptIsInTrimmedAreaOfBREP((ON_Brep*)brep, u, v, 200)) continue;
        
        toolPt=calcToolPoint(srf, u, v, flipNormals, toolRadius);
        
        if (stepDown && toolPt.z>(z-zStep)){
//          printf("Limit Z %lf  ", z);
//          printf("Tool Z %lf  ", toolPt.z);
//          printf("Should rapid over this point...\n");
          if (!isRapid){
            printf("into rapid - u=%lf v=%lf\n", u, v);
            path.append("G0 Z[#<workZ>+#<rapidZ>]");
            isRapid=true;
          }
          continue;
         
        }
        
        if (isRapid){
          printf("out of rapid - u=%lf v=%lf\n", u, v);
          sprintf(line,"G0 X%lf Y%lf",toolPt.x, toolPt.y);
          path.append(line);
          isFirst=true;
          isRapid=false;
        }
        
        if (toolPt.z<z){
          toolPt.z=z;
          takeNextCut=true;
        }
        
        if (isFirst){
          sprintf(line,"G0 X%lf Y%lf",toolPt.x, toolPt.y); path.append(line);
          sprintf(line,"G0 Z[#<workZ>+%lf]", toolPt.z-zStep); path.append(line);
          path.append("F #<zFeed>");
        }
        
        sprintf(line,"G1 X%lf Y%lf Z[#<workZ>+%lf]", toolPt.x, toolPt.y, toolPt.z); path.append(line);

        if (isFirst){
          path.append("F #<stdFeed>");
          isFirst=false;
        }
      }
      path.append("G0 Z[#<workZ>+#<rapidZ>]");
    }
  }
  
  // Take cut around trim loops to finish...
  path.append("(Taking cut around trim loop)");
  for (i=0;i<brep->m_T.Count();i++){
    isFirst=true; // Rapid out and back in each time...
    trm=(ON_Curve*)brep->m_T[i].TrimCurveOf();
    trm->GetDomain(&tMin, &tMax);
    tStep=(tMax-tMin)/100.;
    for (t=tMin;t<tMax+tStep/2;t+=tStep){
      onPt=trm->PointAt(t);
      u=onPt.x; v=onPt.y;
      if (srf->IsAtSingularity(u,v,false)) continue;
      toolPt=calcToolPoint(srf, u, v, flipNormals, toolRadius);
      if (isFirst){
        // Rapid to position and one Z step higher...
        sprintf(line,"G0 X%lf Y%lf",toolPt.x, toolPt.y); path.append(line);
        sprintf(line,"G0 Z[#<workZ>+%lf]", toolPt.z-zStep); path.append(line);
        path.append("F #<zFeed>"); // Feed in slowly
        sprintf(line,"G1 Z[#<workZ>+%lf]", toolPt.z); path.append(line);
        path.append("F #<stdFeed>");
        isFirst=false;
      }
      // take cut as normal...
      sprintf(line,"G1 X%lf Y%lf Z[#<workZ>+%lf]",toolPt.x, toolPt.y, toolPt.z);
      path.append(line);
    }
    // rapid out
    path.append("G0 Z[#<workZ>+#<rapidZ>]");
  }
}


ON_3dPoint surfacingToolpath::calcToolPoint(const ON_Surface *srf, double u, double v, bool flipNormals, double toolRadius){
  ON_3dPoint srfPt, normal, toolPt, toolDelta;
  toolDelta=ON_3dPoint(0, 0, -toolRadius);
  
  srfPt=srf->PointAt(u,v);
  normal=srf->NormalAt(u,v);
  
  if (!flipNormals){
    toolPt=srfPt+normal*toolRadius;
  }else{
    toolPt=srfPt-normal*toolRadius;
  }
  
  return toolPt+toolDelta;
}
