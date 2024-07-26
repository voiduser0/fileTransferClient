#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>

#include "FileTransferClientHandle.h"

class TcpClient  : public QObject
{
	Q_OBJECT

public:
	TcpClient(QObject *parent);
	~TcpClient();

private:
	QTcpSocket *m_tcpClient;

public:
	void ConnectToHost(const QString &serverAddr, const quint16 serverPort);
	void DisconnectFromHost();

private:
	void Close();

private slots:
	void ReadMsg();
	void HandleDisConnect();

public slots:
	void SendBytes(const QByteArray &bytes);

signals:
	void GetFileData(const QByteArray& bytes);
	void SigConnected();
	void SigDisConnect();
};
