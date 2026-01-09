#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 1. Create the manager (ensure DataBaseManager inherits from QWidget!)
    m_dbManager = new DataBaseManager(this,this);
    QWidget *dbWidget = m_dbManager->getEditerWidget();

    // 2. Tell QMainWindow to use this as the main view
    QMainWindow:: setCentralWidget(dbWidget);
}
MainWindow::~MainWindow()
{
    delete ui;
}
