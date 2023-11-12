#ifndef PROCESSGROUP_H
#define PROCESSGROUP_H

#include <QWidget>

class QLabel;
class QPushButton;
class QVBoxLayout;

class ProcessGroup : public QWidget
{
    Q_OBJECT

public:
    ProcessGroup(int pgid, QWidget *parent = nullptr);
    ~ProcessGroup();

private slots:
    void onCreateProcessClicked();
    void closeEvent(QCloseEvent *event) override;  // Thêm dòng này
    void onProcessClosed(int pid);

private:
    class ProcessGroupPrivate;
    ProcessGroupPrivate *d;
    static int processCount;  // Biến đếm số lần nhấn nút "Create Process"

    // PGID của Process Group
    int pgid;

signals:
    void processGroupClosed(int pgid);

};

#endif // PROCESSGROUP_H
