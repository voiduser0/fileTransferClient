#pragma once

#include <QtWidgets/QWidget>
#include <QSettings>
#include <QEvent>

#include "ui_FileTransferClient.h"
#include "TcpClient.h"

QT_BEGIN_NAMESPACE
namespace Ui { class FileTransferClientClass; };
QT_END_NAMESPACE

class FileTransferClient : public QWidget
{
    Q_OBJECT

public:
    FileTransferClient(QWidget *parent = nullptr);
    ~FileTransferClient();

private:
    Ui::FileTransferClientClass* ui;

    TcpClient* m_tcp{};

    QByteArray m_data{};
    QString m_filePath{};

private:
    void HandleConnect();
    void HandleNewFile(int iMsgTypeIndex);

protected:
    void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE;

signals:
    void SendBytes(const QByteArray &bytes);

private slots:
    void on_selPathBtn_clicked();
    void on_openFolderBtn_clicked();
    void on_connectBtn_clicked();
    void GetFileData(const QByteArray& bytes);
    void HandleDisConnect();
};
