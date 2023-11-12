#ifndef PROCESS_H
#define PROCESS_H

#include <QWidget>

class QLabel;
class QPushButton;
class QVBoxLayout;
class QLineEdit;
class QTextEdit;
class QCloseEvent; // Add this line

class Process : public QWidget
{
    Q_OBJECT

public:
    Process(int pid, QWidget *parent = nullptr);
    ~Process();

signals:
    void processClosed(int pid);
    void stopClicked();

private slots:
    void closeEvent(QCloseEvent *event) override;

    // New slots for extended functionalities
    void onStartClicked();
    void onStopClicked();
    void onSaveClicked();
    void onLoadClicked();
    void onReadyReadStandardOutput(); // Add this line
    void onReadyReadStandardError();  // Add this line

private:
    class ProcessPrivate;
    ProcessPrivate *d;

    int pid;
};

#endif // PROCESS_H
