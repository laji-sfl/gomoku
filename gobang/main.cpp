#include <QApplication>
#include "board.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Board gb_board;
    gb_board.show();

    return app.exec();
}
