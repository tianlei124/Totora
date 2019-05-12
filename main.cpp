#include <QApplication>

#include "searchwindow.h"

int main(int argc, char * argv[])
{
    QApplication app(argc, argv);
    searchWindow *searchEdit = new searchWindow;
    searchEdit->show();
    return app.exec();
}
