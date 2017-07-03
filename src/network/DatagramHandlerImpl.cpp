/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "DatagramHandlerImpl.h"

#include "EnvHolder.h"

#include <QLoggingCategory>
#include <QMutexLocker>
#include <QNetworkProxy>
#include <QWeakPointer>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(network)


namespace governikus
{

template<> DatagramHandler* createNewObject<DatagramHandler>()
{
	return new DatagramHandlerImpl;
}


} /* namespace governikus */


quint16 DatagramHandlerImpl::cPort = 24727;


DatagramHandlerImpl::DatagramHandlerImpl()
	: DatagramHandler()
	, mSocket(new QUdpSocket)
{
#ifndef QT_NO_NETWORKPROXY
	mSocket->setProxy(QNetworkProxy::NoProxy);
#endif

	connect(mSocket.data(), &QUdpSocket::readyRead, this, &DatagramHandlerImpl::onReadyRead);

	if (mSocket->bind(cPort))
	{
		qCDebug(network) << "Bound on port:" << mSocket->localPort();
	}
	else
	{
		qCDebug(network) << "Cannot bind socket:" << mSocket->errorString();
	}
}


DatagramHandlerImpl::~DatagramHandlerImpl()
{
	if (isBound())
	{
		qCDebug(network) << "Shutdown socket";
		mSocket->close();
	}
}


bool DatagramHandlerImpl::isBound() const
{
	return mSocket->state() == QAbstractSocket::BoundState;
}


bool DatagramHandlerImpl::send(const QJsonDocument& pData, const QHostAddress& pAddress)
{
	const auto& data = pData.toJson(QJsonDocument::Compact);
	const quint16 remotePort = cPort == 0 ? mSocket->localPort() : cPort;
	if (mSocket->writeDatagram(data.constData(), pAddress, remotePort) != data.size())
	{
		qCCritical(network) << "Cannot write datagram:" << mSocket->error() << '|' << mSocket->errorString();
		return false;
	}
	return true;
}


void DatagramHandlerImpl::onReadyRead()
{
	while (mSocket->hasPendingDatagrams())
	{
		QByteArray datagram;
		QHostAddress addr;

		datagram.resize(static_cast<int>(mSocket->pendingDatagramSize()));
		mSocket->readDatagram(datagram.data(), datagram.size(), &addr);

		QJsonParseError jsonError;
		const auto& json = QJsonDocument::fromJson(datagram, &jsonError);
		if (jsonError.error == QJsonParseError::NoError)
		{
			qCDebug(network) << "Fire new message";
			Q_EMIT fireNewMessage(json, addr);
		}
		else
		{
			qCInfo(network) << "Datagram does not contain valid JSON:" << datagram;
		}
	}
}
