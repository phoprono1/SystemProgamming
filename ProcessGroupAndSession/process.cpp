#include "process.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QInputDialog>

class Process::ProcessPrivate
{
public:
    QVBoxLayout *layout;
    QLabel *processLabel;
    QLabel *pidLabel;

    QPushButton *startButton;
    QPushButton *stopButton;
    QPushButton *saveButton;
    QPushButton *loadButton;
    QTextEdit *outputTextEdit;

    // Add this line to declare savedCommand
    QString savedCommand;

    QStringList commandHistory; // Store the command history
    int currentCommandIndex;    // Index of the current command
    QProcess *process;
};

Process::Process(int pid, QWidget *parent)
    : QWidget(parent), d(new ProcessPrivate), pid(pid)
{
    this->resize(600, 400);  // Thay đổi kích thước theo nhu cầu của bạn
    d->layout = new QVBoxLayout;

    d->processLabel = new QLabel("Process");
    d->processLabel->setAlignment(Qt::AlignCenter);
    d->layout->addWidget(d->processLabel);

    d->pidLabel = new QLabel("PID: " + QString::number(pid));
    d->layout->addWidget(d->pidLabel);

    d->startButton = new QPushButton("Start");
    d->stopButton = new QPushButton("Stop");
    d->saveButton = new QPushButton("Save");
    d->loadButton = new QPushButton("Load");

    d->layout->addWidget(d->startButton);
    d->layout->addWidget(d->stopButton);
    d->layout->addWidget(d->saveButton);
    d->layout->addWidget(d->loadButton);

    connect(d->startButton, &QPushButton::clicked, this, &Process::onStartClicked);
    connect(d->stopButton, &QPushButton::clicked, this, &Process::onStopClicked);
    connect(d->saveButton, &QPushButton::clicked, this, &Process::onSaveClicked);
    connect(d->loadButton, &QPushButton::clicked, this, &Process::onLoadClicked);

    d->outputTextEdit = new QTextEdit;
    d->layout->addWidget(d->outputTextEdit);

    d->process = new QProcess(this);

    // Initialize command history
    d->currentCommandIndex = -1;

    setLayout(d->layout);
}

Process::~Process()
{
    delete d;
}

void Process::closeEvent(QCloseEvent *event)
{
    emit processClosed(pid);
    QWidget::closeEvent(event);
}

void Process::onStopClicked()
{
    d->process->terminate();
    d->outputTextEdit->clear();  // Clear the output text edit
    d->commandHistory.clear();   // Clear the command history
    d->savedCommand.clear();     // Clear the saved command
    emit stopClicked();          // Emit the signal to notify about stop event
}

void Process::onStartClicked()
{
    // Disconnect previous connections to avoid overwriting outputs
    disconnect(d->process, &QProcess::readyReadStandardOutput, this, &Process::onReadyReadStandardOutput);
    disconnect(d->process, &QProcess::readyReadStandardError, this, &Process::onReadyReadStandardError);

    // Get the command from the input line
    QString command;
    bool ok;
    command = QInputDialog::getText(this, "Enter Command", "Command:", QLineEdit::Normal, "", &ok);

    if (!ok || command.isEmpty()) {
        return;
    }

    // Append the new command to the savedCommand
    d->savedCommand.append(command + "\n");

    // Start the process with the given command
    d->process->start(command);

    // Connect the process's signals to update the output text edit
    connect(d->process, &QProcess::readyReadStandardOutput, this, &Process::onReadyReadStandardOutput);
    connect(d->process, &QProcess::readyReadStandardError, this, &Process::onReadyReadStandardError);
}

void Process::onLoadClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Load Command", QString(), "Text Files (*.txt)");

    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            QString loadedCommands = stream.readAll();
            file.close();

            // Disconnect previous connections to avoid overwriting outputs
            disconnect(d->process, &QProcess::readyReadStandardOutput, this, &Process::onReadyReadStandardOutput);
            disconnect(d->process, &QProcess::readyReadStandardError, this, &Process::onReadyReadStandardError);

            // Split the loaded commands into a list, skipping empty parts
            QStringList commandList = loadedCommands.split("\n", Qt::SkipEmptyParts);

            // Clear the output text edit only once, before starting the first process
            d->outputTextEdit->clear();

            // Clear the saved command
            d->savedCommand.clear();

            // Iterate through the commands and execute them
            for (const QString &command : commandList) {
                // Start the process with the loaded command
                d->process->start(command);

                // Connect the process's signals to update the output text edit
                connect(d->process, &QProcess::readyReadStandardOutput, this, &Process::onReadyReadStandardOutput);
                connect(d->process, &QProcess::readyReadStandardError, this, &Process::onReadyReadStandardError);

                // Wait for the process to finish (optional)
                d->process->waitForFinished();

                // Append the loaded command to savedCommand
                d->savedCommand.append(command + "\n");
            }
        }
    }
}

void Process::onSaveClicked()
{
    // Save all entered commands to a file
    QString filePath = QFileDialog::getSaveFileName(this, "Save Commands", QString(), "Text Files (*.txt)");

    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << d->savedCommand; // Save all entered commands
            file.close();
        }
    }
}

void Process::onReadyReadStandardOutput()
{
    // Read the standard output from the process
    QString output = d->process->readAllStandardOutput();

    // Append the output to the command history list
    d->commandHistory.append(output);

    // Display the output in the text edit
    d->outputTextEdit->append(output);
}

void Process::onReadyReadStandardError()
{
    // Read the standard error from the process
    QString error = d->process->readAllStandardError();

    // Append the error to the command history list
    d->commandHistory.append(error);

    // Display the error in the text edit
    d->outputTextEdit->append(error);
}
