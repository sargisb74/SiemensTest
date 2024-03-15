#include <QApplication>
#include "main_wnd.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    MainWnd wnd(nullptr);
    wnd.show();

    return QApplication::exec();
}
