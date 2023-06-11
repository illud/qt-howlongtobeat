
#ifndef HOWLONGTOBEAT_H
#define HOWLONGTOBEAT_H

#include <QMainWindow>
#include <QPixmap>


QT_BEGIN_NAMESPACE
namespace Ui { class HowLongTobeat; }
QT_END_NAMESPACE

class HowLongTobeat : public QMainWindow

{
    Q_OBJECT

public:
    HowLongTobeat(QWidget *parent = nullptr);
    ~HowLongTobeat();

private slots:
    void on_searchBtn_clicked();

private:
    Ui::HowLongTobeat *ui;
};

#endif // HOWLONGTOBEAT_H
