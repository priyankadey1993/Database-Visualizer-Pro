#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "databasemanager.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
class DataBaseManager;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    DataBaseManager *m_dbManager;
};
#endif // MAINWINDOW_H
