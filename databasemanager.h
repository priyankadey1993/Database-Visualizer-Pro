#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QMainWindow>
#include <QObject>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QTableView>

#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlError>
#include <QDebug>
#include <QGroupBox>
#include <QFormLayout>
#include <QStatusBar>
#include <QHeaderView>
#include <QFileInfo>
#include <QWidget>


class DataBaseManagerData;

class DataBaseManager :public QObject
{
    Q_OBJECT
public:

    explicit DataBaseManager(QWidget *parent_widget = nullptr,QObject *parent = nullptr);
    QWidget* getEditerWidget();
    ~DataBaseManager();

public slots:
    void browseDatabaseFile();
    void connectToDatabase();
    void disconnectToDatabase();
    void exportTable();
    void showTableInfo();
    void onTableSelected(int index);

signals:
  void databaseConnected(bool connected);

private:
    QSqlDatabase m_database;
    QSqlTableModel *m_tablemodel;
    QVBoxLayout *m_mainLayout;
    QString m_currentPath;
    QString m_currentTable;

    QComboBox *m_dbTypeCombo;
    QWidget *m_container;

    QLineEdit *m_dbPathEdit;
    QPushButton *m_browseBtn;
    QPushButton *m_connectBtn;
    QPushButton *m_disconnectBtn;
    QPushButton *m_refreshBtn;
    QPushButton *m_exportBtn;
    QPushButton *m_infoBtn;
    QPushButton *m_chartBtn;

    QLabel*m_statusLabel;

    QLabel *m_tableInfoLabel;
    QComboBox *m_tableCombo;
    QTableView *m_tableView;






    void setupUI();
    void setupConnections();
    void createDatabaseConnection();
    bool connectToSQLite(const QString& );
    void loadTables();
    void createChart();
    QStringList getAvailableDatabaseTypes();
    QStringList getTableColumnNames(const QString& tablename);
    bool isColumnNumeric(const QString& tableName, const QString& columnName);



};

#endif // DATABASEMANAGER_H
