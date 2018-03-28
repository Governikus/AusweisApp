/*!
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#include "DatagramHandlerImpl.h"

#include "Env.h"

#include <QLoggingCategory>
#include <QMutexLocker>
#include <QNetworkInterface>
#include <QNetworkProxy>
#include <QWeakPointer>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(network)


namespace governikus
{

template<> DatagramHandler* createNewObject<DatagramHandler*>()
{
	return new DatagramHandlerImpl;
}


template<> DatagramHandler* createNewObject<DatagramHandler*, bool>(bool&& pListen)
{
	return new DatagramHandlerImpl(pListen);
}


} /* namespace governikus */


quint16 DatagramHandlerImpl::cPort = 24727;


DatagramHandlerImpl::DatagramHandlerImpl(bool pListen)
	: DatagramHandler()
	, mSocket(new QUdpSocket)
{
#ifndef QT_NO_NETWORKPROXY
	mSocket->setProxy(QNetworkProxy::NoProxy);
#endif

	connect(mSocket.data(), &QUdpSocket::readyRead, this, &DatagramHandlerImpl::onReadyRead);

	if (!pListen)
	{
		qCDebug(network) << "Skipping binding";
	}
	else if (mSocket->bind(cPort))
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
	if (DatagramHandlerImpl::isBound())
	{
		qCDebug(network) << "Shutdown socket";
		mSocket->close();
	}
}


bool DatagramHandlerImpl::isBound() const
{
	return mSocket->state() == QAbstractSocket::BoundState;
}


bool DatagramHandlerImpl::send(const QJsonDocument& pData)
{
	QVector<QHostAddress> broadcastAddresses;
	const auto& interfaces = QNetworkInterface::allInterfaces();
	for (const QNetworkInterface& interface : interfaces)
	{
		const auto& entries = interface.addressEntries();
		for (const QNetworkAddressEntry& addressEntry : entries)
		{
			const QHostAddress& broadcastAddr = addressEntry.broadcast();
			if (broadcastAddr.isNull())
			{
				continue;
			}
			if (addressEntry.ip().isEqual(QHostAddress::LocalHost, QHostAddress::TolerantConversion))
			{
				continue;
			}

			broadcastAddresses += broadcastAddr;
		}
	}

	if (broadcastAddresses.isEmpty())
	{
		return false;
	}

	for (const QHostAddress& broadcastAddr : qAsConst(broadcastAddresses))
	{
		if (!send(pData, broadcastAddr))
		{
			qDebug() << "Broadcasting to" << broadcastAddr << "failed";
			return false;
		}
	}

	return true;
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
			Q_EMIT fireNewMessage(json, addr);
		}
		else
		{
			static int timesLogged = 0;
			if (timesLogged < 20)
			{
				qCInfo(network) << "Datagram does not contain valid JSON:" << datagram;
				timesLogged++;
			}
		}
	}
}
