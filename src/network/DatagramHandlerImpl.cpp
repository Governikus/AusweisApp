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


} // namespace governikus

quint16 DatagramHandlerImpl::cPort = PortFile::cDefaultPort;

DatagramHandlerImpl::DatagramHandlerImpl(bool pListen, quint16 pPort)
	: DatagramHandler()
	, mSocket(new QUdpSocket)
	, mUsedPort(pPort)
	, mPortFile(QStringLiteral("udp"))
{
#ifndef QT_NO_NETWORKPROXY
	mSocket->setProxy(QNetworkProxy::NoProxy);
#endif

	connect(mSocket.data(), &QUdpSocket::readyRead, this, &DatagramHandlerImpl::onReadyRead);

	if (!pListen)
	{
		qCDebug(network) << "Skipping binding";

		// If --port 0 is given we cannot use this as a client. Automatic port
		// usage is only supported by a server.
		if (mUsedPort == 0)
		{
			mUsedPort = PortFile::cDefaultPort;
			qCWarning(network) << "Client port cannot be 0! Reset to default:" << mUsedPort;
		}
	}
	else if (mSocket->bind(mUsedPort))
	{
		mUsedPort = mSocket->localPort(); // if user provides 0, we need to overwrite it with real value
		mPortFile.handlePort(mUsedPort);
		qCDebug(network) << "Bound on port:" << mUsedPort;
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


bool DatagramHandlerImpl::send(const QByteArray& pData)
{
	return send(pData, 0);
}


bool DatagramHandlerImpl::send(const QByteArray& pData, quint16 pPort)
{
	QVector<QHostAddress> broadcastAddresses;

	const auto& interfaces = QNetworkInterface::allInterfaces();
	for (const QNetworkInterface& interface : interfaces)
	{
		bool skipFurtherIPv6AddressesOnThisInterface = false;

		const auto& entries = interface.addressEntries();
		for (const QNetworkAddressEntry& addressEntry : entries)
		{
			switch (addressEntry.ip().protocol())
			{
				case QAbstractSocket::NetworkLayerProtocol::IPv4Protocol:
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
					break;
				}

				case QAbstractSocket::NetworkLayerProtocol::IPv6Protocol:
				{
					if (skipFurtherIPv6AddressesOnThisInterface)
					{
						continue;
					}

					const QString& scopeId = addressEntry.ip().scopeId();
					if (scopeId.isEmpty())
					{
						continue;
					}

					QHostAddress scopedMulticastAddress = QHostAddress(QStringLiteral("ff02::1"));
					scopedMulticastAddress.setScopeId(scopeId);
					broadcastAddresses += scopedMulticastAddress;

					skipFurtherIPv6AddressesOnThisInterface = true;
					break;
				}

				default:
				{
					qCDebug(network) << "Skipping unknown protocol type:" << addressEntry.ip().protocol();
				}
			}
		}
	}

	if (broadcastAddresses.isEmpty())
	{
		return false;
	}

	for (const QHostAddress& broadcastAddr : qAsConst(broadcastAddresses))
	{
		if (!send(pData, broadcastAddr, pPort))
		{
			qCDebug(network) << "Broadcasting to" << broadcastAddr << "failed";
			return false;
		}
	}

	return true;
}


bool DatagramHandlerImpl::send(const QByteArray& pData, const QHostAddress& pAddress, quint16 pPort)
{
	// If port is 0 we should take our own listening port as destination as other instances
	// should use the same port to receive broadcasts.
	const auto port = pPort > 0 ? pPort : mUsedPort;
	Q_ASSERT(port > 0);

	if (mSocket->writeDatagram(pData.constData(), pAddress, port) != pData.size())
	{
		qCCritical(network) << "Cannot write datagram to address" << pAddress << ':' << mSocket->error() << '|' << mSocket->errorString();
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

		Q_EMIT fireNewMessage(datagram, addr);
	}
}
