#include "TcpClient.h"

#include <qDebug>

TcpClient::TcpClient(QObject *parent)
	: QObject(parent)
{
	m_tcpClient = new QTcpSocket(this);
	connect(m_tcpClient, &QTcpSocket::readyRead, this, &TcpClient::ReadMsg);
	connect(m_tcpClient, &QTcpSocket::disconnected, this, &TcpClient::HandleDisConnect);
}

TcpClient::~TcpClient()
{
	Close();
}

void TcpClient::ConnectToHost(const QString &serverAddr, const quint16 serverPort)
{
	m_tcpClient->connectToHost(QHostAddress(serverAddr), serverPort);
}

void TcpClient::Close()
{
	SendBytes("DISCONNECTION|");
	m_tcpClient->waitForBytesWritten();
	m_tcpClient->disconnectFromHost();
	m_tcpClient->close();
	delete m_tcpClient;
	m_tcpClient = Q_NULLPTR;
}

void TcpClient::HandleDisConnect()
{
	emit SigDisConnect();
}

void TcpClient::SendBytes(const QByteArray& bytes)
{
	SendMsg(m_tcpClient, bytes);
}

void TcpClient::ReadMsg()
{
	HandleMsg(m_tcpClient, [&](const QByteArray& bytes) {
		emit GetFileData(bytes);
		});
}

