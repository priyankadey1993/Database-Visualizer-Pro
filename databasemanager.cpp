#include "databasemanager.h"

#include <QVBoxLayout>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlQuery>
#include<QtCharts/QtCharts>



DataBaseManager::DataBaseManager(QWidget *parent_widget,QObject *parent)
    : QObject(parent),
    m_tablemodel(nullptr),
    m_currentPath("")
{
   m_container = new QWidget(parent_widget);
    qDebug()<<"constructor ";
    setupUI();
    setupConnections();
}


DataBaseManager::~DataBaseManager() {}

void DataBaseManager::setupUI()
{

    m_mainLayout = new QVBoxLayout(m_container);

    QGroupBox *connectionGroup = new QGroupBox("database connection group");
    QVBoxLayout *connectionLayout= new QVBoxLayout(connectionGroup);

    QHBoxLayout *typelayout = new QHBoxLayout();
    typelayout->addWidget(new QLabel("database Type"));

    m_dbTypeCombo = new QComboBox();
    m_dbTypeCombo->addItems(getAvailableDatabaseTypes());
    typelayout->addWidget(m_dbTypeCombo);
    typelayout->addStretch();
    connectionLayout->addLayout(typelayout);

    //sqlite seteing for addibrowseing sqlite
    QHBoxLayout *sqliteLayout = new QHBoxLayout();

    m_dbPathEdit = new QLineEdit();
    m_dbPathEdit->setPlaceholderText("slecte database file... ");

    m_browseBtn =new QPushButton("browse");
    m_browseBtn->setFixedWidth(80);

    sqliteLayout-> addWidget(new QLabel("database file"));
    sqliteLayout-> addWidget(m_dbPathEdit);
    sqliteLayout-> addWidget(m_browseBtn);
    connectionLayout->addLayout(sqliteLayout);

    //buttonlayout
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    m_connectBtn = new QPushButton("connect");
    m_connectBtn->setFixedWidth(80);

    m_disconnectBtn = new QPushButton("disconnect");
    m_disconnectBtn->setFixedWidth(80);
    m_disconnectBtn->setEnabled(false);

    m_refreshBtn = new QPushButton("refresh");
    m_refreshBtn->setFixedWidth(80);
    m_refreshBtn->setEnabled(false);

    m_exportBtn = new QPushButton("export");
    m_exportBtn->setFixedWidth(80);
    m_exportBtn->setEnabled(false);

    m_infoBtn = new QPushButton("show table");
    m_infoBtn->setFixedWidth(80);
    m_infoBtn->setEnabled(false);

    m_chartBtn = new QPushButton("show charts");
    m_chartBtn->setFixedWidth(80);
    m_chartBtn->setEnabled(false);

    buttonLayout->addWidget(m_connectBtn);
    buttonLayout->addWidget(m_disconnectBtn);
    buttonLayout->addWidget(m_refreshBtn);//
    buttonLayout->addWidget(m_exportBtn);//
    buttonLayout->addWidget(m_infoBtn);//
    buttonLayout->addWidget(m_chartBtn);//
    buttonLayout->addStretch();
    connectionLayout->addLayout(buttonLayout);

    m_statusLabel =  new QLabel();
    m_statusLabel->setStyleSheet("QLabel { color: gray; }");
    connectionLayout->addWidget(m_statusLabel);

    m_mainLayout->addWidget(connectionGroup);

    QGroupBox *tableGroup = new QGroupBox("tabel selction group");
    QVBoxLayout *tableLayout= new QVBoxLayout(tableGroup);
    tableLayout->addWidget(new QLabel("select table"));


    m_tableCombo= new QComboBox();
    m_tableCombo->setEnabled(false);
    tableLayout->addWidget(m_tableCombo);

    m_tableInfoLabel = new QLabel();
    m_tableInfoLabel->setStyleSheet("QLabel{color: blue;}");
    tableLayout->addWidget(m_tableInfoLabel);
    tableLayout->addStretch();

    m_mainLayout->addWidget(tableGroup);

    m_tableView =  new QTableView();
    m_tableView->setAlternatingRowColors(true);
    m_tableView->setSortingEnabled(true);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    m_mainLayout->addWidget(m_tableView);
    m_mainLayout->addStretch();


}

QStringList DataBaseManager::getAvailableDatabaseTypes()
{
    return {"SQLite"};
}

QWidget* DataBaseManager::getEditerWidget()
{
    return m_container; // Return the widget we just built
}

void DataBaseManager::browseDatabaseFile()
{
    QString filter;
    if(m_dbTypeCombo->currentText()== "SQLite")
    {
        filter = "SQLite Database Files (*.db,*.sqlite,*.sqlite3);ALL Files (*.*)";
    }else {
        filter = "All Files (*.*)";
    }

    QString fileName = QFileDialog::getOpenFileName(m_container,
                                                    "Select Database File",
                                                    QDir::homePath(),
                                                    filter);

    if (!fileName.isEmpty()) {
        m_dbPathEdit->setText(fileName);
    }

}

void DataBaseManager:: connectToDatabase()
{
    if(m_database.isOpen())
    {
        QMessageBox::information(m_container,"already connected","database is already connected");
        return;

    }
    createDatabaseConnection();
}

void DataBaseManager:: createDatabaseConnection()
{
    QString dbtype=m_dbTypeCombo->currentText();
    bool success = false;

    if(dbtype == "SQLite")
    {
        QString dbPath = m_dbPathEdit->text().trimmed();
        if(dbPath.isEmpty())
        {
            QMessageBox::warning(m_container,"error","plase select correct data base file");
            return;
        }

        QFileInfo fileinfo(dbPath);

        if(!fileinfo.exists())
        {
            QMessageBox::warning(m_container,"error",QString("Database file does not exist:\n%1").arg(dbPath));
            return;
        }

        success = connectToSQLite(dbPath);
    }
    else
    {
        QMessageBox::warning(m_container,"error","plases selct onlly  sqlite");
    }

    if(success)
    {
        m_connectBtn->setEnabled(false);
        m_disconnectBtn->setEnabled(true);
        m_refreshBtn->setEnabled(true);
        m_exportBtn->setEnabled(true);
        m_infoBtn->setEnabled(true);
         m_chartBtn->setEnabled(true);
        m_dbPathEdit->setEnabled(false);
        m_browseBtn->setEnabled(false);

       loadTables();
       emit databaseConnected(true);
    }
    else
    {
        QString error = m_database.lastError().text();
        QMessageBox::warning(m_container,"error",error);
    }
}

bool DataBaseManager::connectToSQLite(const QString &path)
{
    if(!QSqlDatabase::isDriverAvailable("QSQLITE"))
    {
        QMessageBox::warning(m_container, "Driver Error",
                              "QSQLITE driver not available! ") ;
        return false;
    }

    if(QSqlDatabase::contains("sqlte_connection"))
    {
        QSqlDatabase::removeDatabase("sqlte_connection");
    }

    //create new connection
    m_database = QSqlDatabase::addDatabase("QSQLITE","sqlite_connection");
    m_database.setDatabaseName(path);
    m_currentPath = path;
    if (!m_database.open()) {

        QMessageBox::warning(m_container, "database error",
                             "database not opening ") ;
        return false;
    }

    return true;
}

void DataBaseManager::loadTables()
{
    if(!m_database.isOpen())
    {
        return;
    }
    QStringList tables = m_database.tables(QSql::Tables);
    m_tableCombo->clear();
    m_tableCombo->addItems(tables);
    m_tableCombo->setEnabled(true);

    if(tables.empty())
    {
        m_statusLabel->setText("Status:  Database contains no tables");
        m_statusLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
    }
    else
    {
        QString status = QString("Found %1 tables").arg(tables.size());
         m_statusLabel->setText(status);
         m_statusLabel->setStyleSheet("QLabel { color: green; }");
         m_tableCombo->setCurrentIndex(0);
    }
}

 void DataBaseManager::disconnectToDatabase()
{
     if(m_database.isOpen())
        m_database.close();

    if(QSqlDatabase::contains("sqlte_connection"))
    {
        QSqlDatabase::removeDatabase("sqlte_connection");
    }

    m_connectBtn->setEnabled(true);
    m_disconnectBtn->setEnabled(false);
    m_refreshBtn->setEnabled(false);
    m_exportBtn->setEnabled(false);
    m_infoBtn->setEnabled(false);
    m_chartBtn->setEnabled(false);
    m_dbPathEdit->setEnabled(true);
    m_dbPathEdit->clear();
    m_browseBtn->setEnabled(true);
    m_tableCombo->setEnabled(false);
    m_tableCombo->clear();

    if (m_tablemodel) {
        delete m_tablemodel;
        m_tablemodel = nullptr;
    }
}
void DataBaseManager::onTableSelected(int index)
{
    if(index< 0 || !m_database.isOpen())
    {
        return ;
    }

    m_currentTable = m_tableCombo->currentText();

    if(m_tablemodel)
    {
        delete m_tablemodel;
    }

    m_tablemodel = new QSqlTableModel(this,m_database);
    m_tablemodel->setTable(m_currentTable);
    m_tablemodel->setEditStrategy(QSqlTableModel::OnRowChange);

    if(!m_tablemodel->select())
    {
        QString error = m_tablemodel->lastError().text();
        m_statusLabel->setText("failed to load table" + error);
        return;
    }

    m_tableView->setModel(m_tablemodel);
    int rowCount = m_tablemodel->rowCount();
    int colCount = m_tablemodel->columnCount();
    m_tableInfoLabel->setText(
        QString("Rows: %1 | Columns: %2").arg(rowCount).arg(colCount)
        );
    m_statusLabel->setText(QString("Table '%1' loaded with %2 rows").arg(m_currentTable).arg(rowCount));
}


void DataBaseManager::exportTable()
{

}

bool DataBaseManager::isColumnNumeric(const QString& tableName, const QString& columnName)
{
    if (!m_database.isOpen()) return false;
    
    QSqlQuery query(m_database);
    query.prepare(QString("SELECT %1 FROM %2 LIMIT 1").arg(columnName).arg(tableName));
    
    if (!query.exec() || !query.next()) {
        return false;
    }
    
    QVariant value = query.value(0);
    QVariant::Type type = value.type();
    
    return (type == QVariant::Int || 
            type == QVariant::Double || 
            type == QVariant::LongLong ||
            type == QVariant::UInt ||
            type == QVariant::ULongLong);
}

void DataBaseManager::createChart()
{
    if (!m_database.isOpen() || m_currentTable.isEmpty()) {
        QMessageBox::warning(m_container, "Error", "No table selected!");
        return;
    }

    QStringList columns = getTableColumnNames(m_currentTable);
    if (columns.size() < 2) {
        QMessageBox::warning(m_container, "Error", "Table must have at least 2 columns!");
        return;
    }

    // Create dialog for column selection
    QDialog dialog(m_container);
    dialog.setWindowTitle("Create Chart");
    dialog.resize(400, 250);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    // X-axis selection
    QHBoxLayout *xLayout = new QHBoxLayout();
    xLayout->addWidget(new QLabel("X-Axis (Category/Label):"));
    QComboBox *xAxisCombo = new QComboBox();
    xAxisCombo->addItems(columns);
    xLayout->addWidget(xAxisCombo);
    layout->addLayout(xLayout);

    // Y-axis selection
    QHBoxLayout *yLayout = new QHBoxLayout();
    yLayout->addWidget(new QLabel("Y-Axis (Value):"));
    QComboBox *yAxisCombo = new QComboBox();
    yAxisCombo->addItems(columns);
    if (columns.size() > 1) {
        yAxisCombo->setCurrentIndex(1);
    }
    yLayout->addWidget(yAxisCombo);
    layout->addLayout(yLayout);

    // Chart type selection
    QHBoxLayout *typeLayout = new QHBoxLayout();
    typeLayout->addWidget(new QLabel("Chart Type:"));
    QComboBox *chartTypeCombo = new QComboBox();
    chartTypeCombo->addItems({"Bar Chart", "Pie Chart", "Line Chart"});
    typeLayout->addWidget(chartTypeCombo);
    layout->addLayout(typeLayout);

    // Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *okBtn = new QPushButton("Create");
    QPushButton *cancelBtn = new QPushButton("Cancel");
    btnLayout->addStretch();
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);
    layout->addLayout(btnLayout);

    connect(okBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QString xColumn = xAxisCombo->currentText();
        QString yColumn = yAxisCombo->currentText();
        QString chartType = chartTypeCombo->currentText();

        // Fetch data
        QSqlQuery query(m_database);
        query.prepare(QString("SELECT %1, %2 FROM %3").arg(xColumn).arg(yColumn).arg(m_currentTable));

        if (!query.exec()) {
            QMessageBox::warning(m_container, "Error", "Failed to fetch data: " + query.lastError().text());
            return;
        }

        // Store data
        QMap<QString, double> data;
        bool yIsNumeric = isColumnNumeric(m_currentTable, yColumn);

        while (query.next()) {
            QString xValue = query.value(0).toString();
            double yValue = yIsNumeric ? query.value(1).toDouble() : 1.0;

            if (data.contains(xValue)) {
                data[xValue] += yValue;
            } else {
                data[xValue] = yValue;
            }
        }

        if (data.isEmpty()) {
            QMessageBox::warning(m_container, "Error", "No data to display!");
            return;
        }

        // Create chart
        QChart *chart = new QChart();

        if (chartType == "Bar Chart") {
            QBarSet *set = new QBarSet(yColumn);
            QStringList categories;

            for (auto it = data.begin(); it != data.end(); ++it) {
                *set << it.value();
                categories << it.key();
            }

            QBarSeries *series = new QBarSeries();
            series->append(set);
            chart->addSeries(series);

            QBarCategoryAxis *axisX = new QBarCategoryAxis();
            axisX->append(categories);
            chart->addAxis(axisX, Qt::AlignBottom);
            series->attachAxis(axisX);

            QValueAxis *axisY = new QValueAxis();
            chart->addAxis(axisY, Qt::AlignLeft);
            series->attachAxis(axisY);

        } else if (chartType == "Pie Chart") {
            QPieSeries *series = new QPieSeries();
            for (auto it = data.begin(); it != data.end(); ++it) {
                series->append(it.key(), it.value());
            }
            chart->addSeries(series);

        } else if (chartType == "Line Chart") {
            QLineSeries *series = new QLineSeries();
            int index = 0;
            for (auto it = data.begin(); it != data.end(); ++it) {
                series->append(index++, it.value());
            }
            chart->addSeries(series);
            chart->createDefaultAxes();
        }

        chart->setTitle(QString("%1 vs %2").arg(xColumn).arg(yColumn));
        chart->legend()->setVisible(true);

        // Display chart
        QChartView *chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);

        QDialog chartDialog(m_container);
        chartDialog.setWindowTitle("Chart Viewer");
        chartDialog.resize(800, 600);

        QVBoxLayout *chartLayout = new QVBoxLayout(&chartDialog);
        chartLayout->addWidget(chartView);

        chartDialog.exec();
    }
}

void DataBaseManager::showTableInfo()
{

    if(!m_database.isOpen()||m_currentTable.isEmpty())
    {
        return ;
    }

    QStringList columns = getTableColumnNames(m_currentTable);

    QString info = QString("Table : %1\n").arg(m_currentTable);
    info += QString("Columns: %1\n").arg(columns.size());
    info+="column list:\n";

    for(int i=0;i<columns.size();i++)
    {
      info += QString("  %1. %2\n").arg(i+1).arg(columns[i]);
    }

    if(m_tablemodel)
    {
        info += QString("\nRow Count: %1").arg(m_tablemodel->rowCount());
    }

    QMessageBox::information(m_container, "Table Information", info);
}

QStringList DataBaseManager:: getTableColumnNames(const QString& tablename)
{
    QStringList columnNames;
    QSqlRecord record = m_database.record(tablename);

    for(auto i=0;i<record.count();i++)
    {
        columnNames<< record.fieldName(i);
    }
    return columnNames;
}

void DataBaseManager::setupConnections()
{
    connect(m_browseBtn,&QPushButton::clicked,this,&DataBaseManager::browseDatabaseFile);
    connect(m_connectBtn,&QPushButton::clicked,this,&DataBaseManager::connectToDatabase);
    connect(m_disconnectBtn,&QPushButton::clicked,this,&DataBaseManager::disconnectToDatabase);
    connect(m_tableCombo,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&DataBaseManager::onTableSelected);
    connect(m_infoBtn,&QPushButton::clicked,this,&DataBaseManager::showTableInfo);
    connect(m_exportBtn,&QPushButton::clicked,this,&DataBaseManager::exportTable);
    connect(m_chartBtn,&QPushButton::clicked,this,&DataBaseManager::createChart);
}
