#ifndef SURFACECORRECTION_H
  #define SURFACECORRECTION_H

#include <QObject>
#include "rcamObject.h"
#include "odPoint.h"

class surfaceCorrection : public rcamObject
{
  Q_OBJECT
  public:
    surfaceCorrection(rcamObject *parent);
    double correctZ(double x, double y);
    
  public slots:
    void readProbeFile();
    
  protected:
    void createMenu();
    virtual void readXMLElement(QDomElement element);
    virtual void writeCustomXML(QIODevice *xml);

  private:
    QList<odPoint> probe;
};
#endif
