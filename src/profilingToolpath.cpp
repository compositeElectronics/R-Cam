#include "profilingToolpath.h"

profilingToolpath::profilingToolpath(machineSettings *settings, const ONX_Model* modelGeom=0) : genericToolpath(settings, modelGeom, "profilingToolpath"){
  setting.append(new editableSetting(QString("zStep"), QString("Z Step"),   QVariant((double)-0.5), QVariant(-100), QVariant(-0.0001)));
  setting.append(new editableSetting(QString("step"), QString("Span Step"), QVariant((double)0.1), QVariant((double)0.001), QVariant((double)1)));
  cutDepth=-13.0;
  nCuts=4;
  createSettingsTable();
}

void profilingToolpath::polishTreeLabel(){
  treeItem->setText(0,QString("Profiling Toolpath"));  
}

void profilingToolpath::calcToolPath(const ON_Curve* curve){
  // The profiling path is very similar to the engrave path, except that it plunges to the next Z step until it reaches the curve
  // instead of maintaining constant depth.
  
  // If you're feeling brave set the zStep to be greater magnitude than the curve z (i.e. a curve at -1mm could have a step of -5mm)
  // In which case you'll just get the curve profile (and likely crash the CNC machine).
  bool takeNextCut;
  double tStart, tEnd;
  double t, tStep, z, zStep;
  char line[256];
  ON_3dPoint onPt;
  
  curve->GetDomain(&tStart, &tEnd);
  tStep=findSettingValue("step").toDouble();
  zStep=findSettingValue("zStep").toDouble();
  
  z=0;
  takeNextCut=true;
  while (takeNextCut){
    z+=zStep;
    onPt=curve->PointAt(tStart);
    sprintf(line,"G0 Z#<safeZ>\n"); path.append(line);
    sprintf(line,"G0 X%lf Y%lf",onPt.x, onPt.y); path.append(line);
    sprintf(line,"G0 Z[#<workZ>+#<rapidZ>]\n"); path.append(line);
    takeNextCut=false;
    printf("Taking cut at Z=%lf\n",z);
    
    for (t=tStart;t<tEnd+tStep/10.;t+=tStep){
//      printf("t %lf of %lf to %lf\n",t, tStart, tEnd);
      onPt=curve->PointAt(t);
      sprintf(line,"G1 X%lf Y%lf",onPt.x, onPt.y);
      if (onPt.z>z){
        sprintf(line,"%s Z[#<workZ>+%lf]", line, onPt.z);
      }else{
        sprintf(line,"%s Z[#<workZ>+%lf]", line, z);
        takeNextCut=true;
//        printf("Must take next cut\n");
      }
      path.append(line);
    }
  }
}
