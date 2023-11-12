// processgroup.cpp
#include "processgroup.h"
#include "process.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <sys/types.h>
#include <unistd.h>
#include "processapi.h"

int ProcessGroup::processCount = 0; // Khởi tạo biến tĩnh

class ProcessGroup::ProcessGroupPrivate
{
public:
    QVBoxLayout *layout;
    QLabel *processGroupLabel;
    QListWidget *processListWidget;  // Thêm QListWidget
};

ProcessGroup::ProcessGroup(int pgid, QWidget *parent)
    : QWidget(parent), d(new ProcessGroupPrivate), pgid(pgid)
{
    d->layout = new QVBoxLayout;

    // Thêm label "Process Group"
    d->processGroupLabel = new QLabel("Process Group");
    d->processGroupLabel->setAlignment(Qt::AlignCenter);
    d->layout->addWidget(d->processGroupLabel);

    // Thêm label "PGID:"
    QLabel *pgidLabel = new QLabel("PGID: " + QString::number(pgid));
    d->layout->addWidget(pgidLabel);

    // Thêm QListWidget để hiển thị danh sách các quy trình
    d->processListWidget = new QListWidget;
    d->layout->addWidget(d->processListWidget);

    QPushButton *createProcessButton = new QPushButton("Create Process");
    connect(createProcessButton, &QPushButton::clicked, this, &ProcessGroup::onCreateProcessClicked);

    d->layout->addWidget(createProcessButton);

    setLayout(d->layout);
}

ProcessGroup::~ProcessGroup()
{
    delete d;
}

void ProcessGroup::onCreateProcessClicked()
{
    // Lấy PID của quy trình con sử dụng API (không cần fork)
    int pid;

    if (processCount >= 1) {
        // Nếu processCount lớn hơn 1, sử dụng fork để tạo pid mới
        pid = fork();
        if (pid == 0) {
            // Trong quy trình con
            qDebug() << "Child process created with PID: " << getpid();
            // TODO: Thêm mã lệnh cho quy trình con ở đây
            exit(0);  // Kết thúc quy trình con
        } else if (pid < 0) {
            // Xử lý lỗi nếu fork thất bại
            qDebug() << "Failed to create child process.";
            return;
        }
    } else {
        // Nếu processCount bằng hoặc nhỏ hơn 1, sử dụng API để lấy pid mới
        pid = ProcessAPI::getPID();
    }

    // Tăng biến đếm mỗi khi nút được nhấn
    processCount++;
    qDebug() << "Number of processes created: " << processCount;

    // Mở cửa sổ Process với pid vừa được tạo
    Process *process = new Process(pid, nullptr);

    // Kết nối signal từ Process để xử lý sự kiện khi Process đóng
    connect(process, &Process::processClosed, this, &ProcessGroup::onProcessClosed);

    process->show();

    // Thêm thông tin quy trình vào danh sách
    QString processInfo = QString("Process %1 (PID: %2)").arg(processCount).arg(pid);
    d->processListWidget->addItem(processInfo);
}

void ProcessGroup::onProcessClosed(int pid) {
    // Tìm và loại bỏ PID khỏi danh sách
    for (int i = 0; i < d->processListWidget->count(); ++i) {
        QListWidgetItem *item = d->processListWidget->item(i);
        QString itemText = item->text();
        if (itemText.contains("PID: " + QString::number(pid))) {
            delete item; // Loại bỏ item khỏi danh sách
            break;
        }
    }
}

void ProcessGroup::closeEvent(QCloseEvent *event) {
    // Phát tín hiệu với PGID hiện tại
    emit processGroupClosed(pgid);
    QWidget::closeEvent(event);
}
