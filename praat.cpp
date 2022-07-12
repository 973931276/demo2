#include "praat.h"
#include "ui_praat.h"

praat::praat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::praat)
{
    ui->setupUi(this);
}

praat::~praat()
{
    delete ui;
}
