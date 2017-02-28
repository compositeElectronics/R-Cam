#include <QApplication>
#include "mainWindow.h"

int main(int argc, char * argv[]){
  QApplication app(argc,argv);

  app.setApplicationName("R-CAM");
  app.setApplicationVersion("v1.0-0");
  
  mainWindow *window = new mainWindow();
  window->show();
  
  return app.exec();
}
