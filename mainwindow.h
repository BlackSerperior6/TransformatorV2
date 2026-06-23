#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_CreateConnection_clicked();

    void on_RemoveConnection_clicked();

    void on_ClearErrorLogs_clicked();

    void on_ConnectionsList_itemDoubleClicked(QListWidgetItem *item);

    void WriteErrorsToLog(quint32 connectionId, const QString& message);

    void on_ConnectionsList_currentRowChanged(int currentRow);

private:
    int currentConListIndex;
    quint32 connectionCounter;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
