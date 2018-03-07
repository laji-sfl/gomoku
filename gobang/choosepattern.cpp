#include "choosepattern.h"
#include <QVBoxLayout>
#include <QPushButton>

ChoosePattern::ChoosePattern(QWidget *parent) : QDialog(parent)
{
    QVBoxLayout *lay = new QVBoxLayout(this);
    QPushButton *b1,*b2;
    lay->addWidget(b1 = new QPushButton("人机对战"));
    lay->addWidget(b2 = new QPushButton("网络对战"));

    connect(b1, SIGNAL(clicked()), this, SLOT(slotClicked1()));
    connect(b2, SIGNAL(clicked()), this, SLOT(slotClicked2()));
}

void ChoosePattern::slotClicked1()
{
    num = '1';  //人机
    accept();
}

void ChoosePattern::slotClicked2()
{
    num = '2';  //网络
    accept();
}

