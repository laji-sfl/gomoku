#ifndef CHOOSEPATTERN_H
#define CHOOSEPATTERN_H

#include <QWidget>
#include <QDialog>

class ChoosePattern : public QDialog
{
    Q_OBJECT
public:
    explicit ChoosePattern(QWidget *parent = 0);
    char num;
public slots:
    void slotClicked1();
    void slotClicked2();
};

#endif // CHOOSEPATTERN_H
