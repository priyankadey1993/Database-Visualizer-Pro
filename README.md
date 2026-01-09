The Qt SQLite Database visulatizer pro is a desktop application built using the Qt Framework (C++/Qt Widgets) that allows users to connect to, browse, and manage SQLite databases. It provides a user-friendly graphical interface for opening existing SQLite database files, exploring their tables, and viewing table data in a structured way without manually running SQL commands.

This tool simplifies interacting with SQLite databases by integrating Qt’s QSqlDatabase, QSqlTableModel, and QTableView classes to handle database connectivity, data modeling, and table visualization.

Key Features:
🔗 Database Connection
Supports connection to SQLite databases via the QSQLITE driver.
Users can browse and select a database file using a file dialog.
Displays the current connection status and the path of the connected database.
📋 Table Management
Automatically lists available database tables after connecting.
Allows users to select a table and view its rows and columns interactively.
Provides information such as number of rows and columns in the selected table.
📊 Table Data Viewing
Uses QSqlTableModel to dynamically load and display data.
Displays table content in a QTableView with:
Alternating row colors
Sortable columns
Row selection
ℹ️ Table Information
Displays detailed information about a selected table:
Table name
Number of columns and rows
Column names
🗃️ Database Control
Buttons for:
Connect / Disconnect
Refresh tables
Export data (placeholder for future functionality)
Show table info
🧱 UI Components
Built using Qt Widgets:
QVBoxLayout, QHBoxLayout, QGroupBox for organized layouts
QComboBox for database and table selections
QPushButton for user actions
QLabel for displaying dynamic status messages
