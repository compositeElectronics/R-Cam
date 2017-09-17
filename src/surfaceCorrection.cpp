#include "surfaceCorrection.h"
#include <QFileDialog>
#include <QFile>
#include <math.h>
#include "odMatrix.h"

surfaceCorrection::surfaceCorrection(rcamObject *parent) : rcamObject(parent, "srfCorrection")
{
  createMenu();
  createSettingsTable();
}

void surfaceCorrection::createMenu(){
//  int i;
  QVariant v;
  
  QStringList processNames;
  v.setValue((void *)this);
  treeItem->setData(0, Qt::UserRole, v);
  
  treeItemMenu = new QMenu(p_type);
  objectAction.append(treeItemMenu->addAction("Load probe file"));
  connect(objectAction.last(), SIGNAL(triggered()), this, SLOT(readProbeFile()));
}

void surfaceCorrection::readProbeFile(){
  QString fileName;
  QString line;
  QStringList data;
  
  fileName = QFileDialog::getOpenFileName(0, "Load Probe File", "", "Probe files (*.txt)");
  if (fileName.isEmpty()) return;
  
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

  while (!file.atEnd()) {
    line = file.readLine();
    data=line.split(" ",QString::SkipEmptyParts);
    probe.append(odPoint(data[0].toDouble(), data[1].toDouble(), data[2].toDouble()));
  }
  
  correctZ(22,37);
}

double surfaceCorrection::correctZ(double x, double y){
  int i, j, n1, n2, n3, n4;
  QList<int> failList;
  double dist, near1=1e6, near2=1e6, near3=1e6, D, zDiff;
  odPoint U, V, N; // Vectors
  bool fail;
  // Find the closest two points...
  
  for (i=0;i<probe.count();i++){
    dist=sqrt(pow(x-probe[i].x,2)+pow(y-probe[i].y,2));
    if (dist<near1){
      near1=dist;
      n1=i;
    }
  }
  for (i=0;i<probe.count();i++){
    dist=sqrt(pow(x-probe[i].x,2)+pow(y-probe[i].y,2));
    if (dist<near2 && i!=n1 && (fabs(probe[i].x-probe[n1].x)>1e-6 || fabs(probe[i].x-probe[n1].x)>1e-6)){
      near2=dist;
      n2=i;
    }
  }

  printf("     Closest point %i - %lf mm\n %lf %lf %lf\n", n1, near1, probe[n1].x, probe[n1].y, probe[n1].z);
  printf("Next Closest point %i - %lf mm\n %lf %lf %lf\n", n2, near2, probe[n2].x, probe[n2].y, probe[n2].z);
  
  n4=-1;
  while (fabs(N.z)<1e-6){
    near3=1e6;
    for (i=0;i<probe.count();i++){
      dist=sqrt(pow(x-probe[i].x,2)+pow(y-probe[i].y,2));
      if (dist<near3 && i!=n1 && i!=n2){
        fail=false;
        for (j=0;j<failList.count();j++){
          if (i==failList[j]) fail=true;
        }
        if (!fail){
          near3=dist;
          n3=i;
        }
      }
    }
    printf("Next Closest point %i - %lf mm\n %lf %lf %lf\n", n3, near3, probe[n3].x, probe[n3].y, probe[n3].z);
  
    // Vector U between first and second points.
    U = probe[n2] - probe[n1];
    // Vector V between first and third points.
    V = probe[n3] - probe[n1];
    N = U.crossProduct(V);
  
//  printf("U = %lf %lf %lf\n", U.x, U.y, U.z);
//  printf("V = %lf %lf %lf\n", V.x, V.y, V.z);
    printf("Cross product = %lf %lf %lf\n",N.x, N.y, N.z);
    failList.append(n3);
  }
  D=(N*probe[n1]).x+(N*probe[n1]).y+(N*probe[n1]).z;
  
  zDiff=(D-N.x*x-N.y*y)/N.z;
  printf("zDiff=%lf\n", zDiff);
  return zDiff;
}

void surfaceCorrection::readXMLElement(QDomElement element){
  if (element.tagName().toLower()=="label") treeItem->setText(0,QString(element.text()));
}

void surfaceCorrection::writeCustomXML(QIODevice *xml){
  spaceXML(xml,true); xml->write(QString("<label>"+treeItem->text(0)+"</label>\n").toLatin1());
}
