#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>

class ProcessGroup;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onCreateProcessGroupClicked();
    void onProcessGroupClosed(int pgid);

private:
    class MainWindowPrivate;
    MainWindowPrivate *d;

    QListWidget *processGroupList;  // Widget hiển thị danh sách Process Group
    QList<ProcessGroup *> processGroups;
};

#endif // MAINWINDOW_H
