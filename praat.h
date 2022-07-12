#ifndef PRAAT_H
#define PRAAT_H

#include <QWidget>

namespace Ui {
class praat;
}

class praat : public QWidget
{
    Q_OBJECT

public:
    explicit praat(QWidget *parent = 0);
    ~praat();

private:
    Ui::praat *ui;
};

#endif // PRAAT_H
