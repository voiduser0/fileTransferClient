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
	QString fileName = QFileDialog::getExistingDirectory(this, QStringLiteral("ѡ���ļ�����·��"));
	ui->selPath->setText(fileName);
}

void FileTransferClient::on_openFolderBtn_clicked()
{
	QString folderPath = ui->selPath->text();
	QDir folderDir(folderPath);

	if (folderDir.exists()) 
		QDesktopServices::openUrl(QUrl::fromLocalFile(folderPath));
	else 
		qInfo() << QStringLiteral("�ļ��в�����");
}

void FileTransferClient::on_connectBtn_clicked()
{
	QString ip = ui->ipLEdit->text();

	if (ip.isEmpty())
	{
		QMessageBox msgBox(QMessageBox::Warning,
			QStringLiteral("������IP��ַΪ��"), QStringLiteral("�����������IP��ַ"));
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
	ui->historyInfo->append(QStringLiteral("�ɹ����ӵ�������"));
	ui->connectStatusLabel->setText(QStringLiteral("����״̬��������"));
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
	ui->historyInfo->append(QStringLiteral("�ѽ��գ�") + fileName);
	emit SendBytes(QString("GETFILE|").toLocal8Bit());
	m_data.clear();
}

void FileTransferClient::HandleDisConnect()
{
	ui->connectBtn->setEnabled(true);
	ui->historyInfo->append(QStringLiteral("�ѶϿ�������������"));
	ui->connectStatusLabel->setText(QStringLiteral("����״̬��δ����"));
}

void FileTransferClient::closeEvent(QCloseEvent* event)
{
	QSettings settings("Path.ini", QSettings::IniFormat);
	if (!ui->selPath->text().isEmpty())
	{
		settings.setValue("path", ui->selPath->text());
	}
}