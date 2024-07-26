#include "FileTransferClient.h"
#include <QDir>
#include <QFile>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>

FileTransferClient::FileTransferClient(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::FileTransferClientClass())
{
	ui->setupUi(this);

	resize(800, 800);
	m_tcp = new TcpClient(this);

	connect(m_tcp, &TcpClient::GetFileData, this, &FileTransferClient::GetFileData);
	connect(m_tcp, &TcpClient::SigDisConnect, this, &FileTransferClient::HandleDisConnect);
	connect(this, &FileTransferClient::SendBytes, m_tcp, &TcpClient::SendBytes);

	QSettings settings("Path.ini", QSettings::IniFormat);
	ui->selPath->setText(settings.value("path").toString());
}

FileTransferClient::~FileTransferClient()
{
	delete ui;
}

void FileTransferClient::on_selPathBtn_clicked()
{
	QString fileName = QFileDialog::getExistingDirectory(this, QStringLiteral("选择文件保存路径"));
	ui->selPath->setText(fileName);
}

void FileTransferClient::on_openFolderBtn_clicked()
{
	QString folderPath = ui->selPath->text();
	QDir folderDir(folderPath);

	if (folderDir.exists()) 
		QDesktopServices::openUrl(QUrl::fromLocalFile(folderPath));
	else 
		qInfo() << QStringLiteral("文件夹不存在");
}

void FileTransferClient::on_connectBtn_clicked()
{
	QString ip = ui->ipLEdit->text();

	if (ip.isEmpty())
	{
		QMessageBox msgBox(QMessageBox::Warning,
			QStringLiteral("服务器IP地址为空"), QStringLiteral("请输入服务器IP地址"));
		msgBox.exec();
		return;
	}
	m_tcp->ConnectToHost(ip, 9999);
	QString qsMsg = QString("NEWCONNECTION|");
	emit SendBytes(qsMsg.toLocal8Bit());
}

void FileTransferClient::GetFileData(const QByteArray& bytes)
{
	m_data = bytes;
	int msgTypeIndex = m_data.indexOf("|");
	QByteArray baMsgType = m_data.left(msgTypeIndex);
	if (baMsgType.contains("SUCCESSFUL"))
	{
		HandleConnect();
	}
	else if (baMsgType.contains("NEWFILE"))
	{
		HandleNewFile(msgTypeIndex);
	}
}

void FileTransferClient::HandleConnect()
{
	ui->connectBtn->setEnabled(false);
	ui->historyInfo->append(QStringLiteral("成功连接到服务器"));
	ui->connectStatusLabel->setText(QStringLiteral("连接状态：已连接"));
}

void FileTransferClient::HandleNewFile(int iMsgTypeIndex)
{
	if (ui->selPath->text().isEmpty())
		return;

	m_data.remove(0, iMsgTypeIndex + 1);
	iMsgTypeIndex = m_data.indexOf("|");
	QString fileName = QString::fromLocal8Bit(m_data.left(iMsgTypeIndex));
	m_data = m_data.mid(iMsgTypeIndex + 1);
	m_filePath = ui->selPath->text() + "/" + fileName;
	QFile sFile(m_filePath);
	sFile.open(QIODevice::WriteOnly);
	sFile.write(m_data, m_data.size());
	sFile.close();
	ui->historyInfo->append(QStringLiteral("已接收：") + fileName);
	emit SendBytes(QString("GETFILE|").toLocal8Bit());
	m_data.clear();
}

void FileTransferClient::HandleDisConnect()
{
	ui->connectBtn->setEnabled(true);
	ui->historyInfo->append(QStringLiteral("已断开服务器的连接"));
	ui->connectStatusLabel->setText(QStringLiteral("连接状态：未连接"));
}

void FileTransferClient::closeEvent(QCloseEvent* event)
{
	QSettings settings("Path.ini", QSettings::IniFormat);
	if (!ui->selPath->text().isEmpty())
	{
		settings.setValue("path", ui->selPath->text());
	}
}