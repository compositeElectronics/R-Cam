#include "profilingToolpath.h"

profilingToolpath::profilingToolpath(machineSettings *settings, const ONX_Model* modelGeom=0) : genericToolpath(settings, modelGeom, "profilingToolpath"){
  setting.append(new editableSetting(QString("zStep"), QString("Z Step"), QVariant((double)-0.5), QVariant(0), QVariant(1000)));
  
  cutDepth=-13.0;
  nCuts=4;
  createSettingsTable();
}

void profilingToolpath::polishTreeLabel(){
  treeItem->setText(0,QString("Profiling Toolpath"));  
}

void profilingToolpath::calcToolPath(const ON_Curve* curve){
  // The engrave path is really easy, as you just follow the curve path exactly, moving the cutter down at the start and up at the end.
  // Flatten Z to safe height (assumed +5) or cut depth (assumed -0.5) to only present XY data.
  int i, iSteps=50000, j;
  double tStart, tEnd;
  double t, tStep;
  double z, zStep;
  ON_3dPoint onPt;
  
  path.clear();
  
  curve->GetDomain(&tStart, &tEnd);
  tStep=(tEnd-tStart)/(double)(iSteps-1);
  
  zStep=cutDepth/nCuts;
  
  for (j=1;j<=nCuts;j++){
    z=(double)j*zStep;
    onPt=curve->PointAt(tStart);
    path.append(odPoint(onPt.x, onPt.y, machine->safeZ()));
    for (i=0;i<iSteps;i++){
      t=(double)i*tStep+tStart;
      onPt=curve->PointAt(t);
      path.append(odPoint(onPt.x, onPt.y, z));
    }
    path.append(odPoint(onPt.x, onPt.y, machine->safeZ()));
  }
}

void profilingToolpath::printPath(){
  int i;
  printf("G0 Z%lf\n",machine->safeZ());
  if (path.count()>0){
    printf("G0 X%lf Y%lf\n",path[0].x, path[0].y);
    printf("G0 Z%lf\n",machine->workpieceZ());
    printf("G1 Z%lf\n", path[0].z);
    for (i=0;i<path.count();i++) printf("G1 X%lf Y%lf Z%lf\n",path[i].x, path[i].y, path[i].z);
  }
  printf("G0 Z%lf\n",machine->safeZ());
}
