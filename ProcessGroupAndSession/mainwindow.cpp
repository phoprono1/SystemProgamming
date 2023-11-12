#include "mainwindow.h"
#include "processapi.h"
#include "processgroup.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidgetItem>

class MainWindow::MainWindowPrivate
{
public:
    QVBoxLayout *layout;
    QLabel *sessionLabel;
    QLabel *sidLabel;
    QListWidget *processGroupList;
    ProcessAPI processAPI;  // Thêm một đối tượng ProcessAPI
    int processGroupCount;   // Biến đếm quy trình nhóm
    int pgidCount;           // Biến đếm PGID
    QMap<int, int> pidCountMap;  // Lưu trữ số lượng PID cho mỗi PGID
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), d(new MainWindowPrivate)
{
    d->layout = new QVBoxLayout;

    d->sessionLabel = new QLabel("SESSION");
    d->sessionLabel->setAlignment(Qt::AlignCenter);
    d->layout->addWidget(d->sessionLabel);

    // Sử dụng đối tượng ProcessAPI để gọi hàm thành viên
    d->sidLabel = new QLabel("SID: " + QString::number(d->processAPI.getSID()));
    d->layout->addWidget(d->sidLabel);

    d->processGroupList = new QListWidget;
    d->layout->addWidget(d->processGroupList);

    QPushButton *createProcessGroupButton = new QPushButton("Create Process Group");
    connect(createProcessGroupButton, &QPushButton::clicked, this, &MainWindow::onCreateProcessGroupClicked);
    d->layout->addWidget(createProcessGroupButton);

    QWidget *centralWidget = new QWidget;
    centralWidget->setLayout(d->layout);
    setCentralWidget(centralWidget);

    // Khởi tạo biến đếm quy trình nhóm và PGID
    d->processGroupCount = 0;
    d->pgidCount = 0;
}

MainWindow::~MainWindow()
{
    delete d;
}

void MainWindow::onCreateProcessGroupClicked()
{
    // Tăng biến đếm mỗi khi nút được nhấn
    d->processGroupCount++;

    // Tăng giá trị PGID nếu processGroupCount lớn hơn 1
    if (d->processGroupCount > 1) {
        d->pgidCount++;
    }

    // Tạo quy trình con mới
    pid_t pgid = d->processAPI.getPGID() + d->pgidCount;

    // Hiển thị thông tin của quy trình con
    ProcessGroup *processGroup = new ProcessGroup(pgid, nullptr);
    processGroup->show();

    // Thêm Process Group mới vào danh sách
    QListWidgetItem *item = new QListWidgetItem("Process Group " + QString::number(d->processGroupCount) + " (PGID: " + QString::number(pgid) + ")");

    // Thiết lập data cho item
    QVariant variant;
    variant.setValue(processGroup);
    item->setData(Qt::UserRole, variant);

    d->processGroupList->addItem(item);

    // Kết nối signal từ ProcessGroup để xử lý sự kiện khi ProcessGroup đóng
    connect(processGroup, &ProcessGroup::processGroupClosed, this, &MainWindow::onProcessGroupClosed);

    // Đặt số lượng PID cho PGID này là 0 ban đầu
    d->pidCountMap[pgid] = 0;
}

void MainWindow::onProcessGroupClosed(int pgid) {
    // Tìm và loại bỏ PGID khỏi danh sách
    for (int i = 0; i < d->processGroupList->count(); ++i) {
        QListWidgetItem *item = d->processGroupList->item(i);
        QString itemText = item->text();
        if (itemText.contains("PGID: " + QString::number(pgid))) {
            // Lấy số lượng PID hiện tại cho PGID này
            int pidCount = d->pidCountMap[pgid];
            // Hiển thị số lượng PID trong thông tin item
            item->setText("Process Group " + QString::number(d->processGroupCount) + " (PGID: " + QString::number(pgid) + ", PID Count: " + QString::number(pidCount) + ")");
            // Loại bỏ item khỏi danh sách
            delete item;
            // Loại bỏ PGID khỏi QMap
            d->pidCountMap.remove(pgid);
            break;
        }
    }
}
