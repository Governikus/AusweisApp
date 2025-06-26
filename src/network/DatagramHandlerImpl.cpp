/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#include "DatagramHandlerImpl.h"

#include "Env.h"

#include <QLoggingCategory>
#include <QMutexLocker>
#include <QNetworkDatagram>
#include <QNetworkProxy>
#include <QWeakPointer>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(network)


namespace governikus
{

constexpr QLatin1StringView ipv6MulticastAddress("ff02::1");

template<> DatagramHandler* createNewObject<DatagramHandler*>()
{
	return new DatagramHandlerImpl();
}


template<> DatagramHandler* createNewObject<DatagramHandler*, bool>(bool&& pEnableListening)
{
	return new DatagramHandlerImpl(pEnableListening);
}


} // namespace governikus


DatagramHandlerImpl::DatagramHandlerImpl(bool pEnableListening, quint16 pPort)
	: DatagramHandler(pEnableListening)
	, mSocket()
	, mMulticastLock()
	, mAllEntries()
	, mFailedAddresses()
	, mUsedPort(pPort)
	, mPortFile(QStringLiteral("udp"))
	, mEnableListening(pEnableListening)
{
	resetSocket();

#if defined(Q_OS_IOS)
	if (pEnableListening)
	{
		checkNetworkPermission();
	}
#endif

}


void DatagramHandlerImpl::resetSocket()
{
	if (DatagramHandlerImpl::isBound())
	{
		mSocket->close();
	}

	mSocket.reset(new QUdpSocket());
	mSocket->setProxy(QNetworkProxy::NoProxy);

	connect(mSocket.data(), &QUdpSocket::readyRead, this, &DatagramHandlerImpl::onReadyRead);

	if (!mEnableListening)
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
#ifndef Q_OS_MACOS
		if (!mSocket->joinMulticastGroup(QHostAddress(ipv6MulticastAddress)))
		{
	#ifdef Q_OS_WIN
			qCDebug(network) << "Could not join multicast group:" << mSocket->errorString();
	#endif
		}
#endif
		if (!mSocket->joinMulticastGroup(QHostAddress(QStringLiteral("ff02::178"))))
		{
			qCDebug(network) << "Could not join multicast group:" << mSocket->errorString();
		}

		mMulticastLock.reset(new MulticastLock());
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
	return mSocket && mSocket->state() == QAbstractSocket::BoundState;
}


QList<QNetworkAddressEntry> DatagramHandlerImpl::getAllBroadcastEntries() const
{
	QList<QNetworkAddressEntry> broadcastEntries;

	const auto& allInterfaces = QNetworkInterface::allInterfaces();
	for (const QNetworkInterface& interface : allInterfaces)
	{
		if (!isValidBroadcastInterface(interface))
		{
			continue;
		}

		const auto& entries = interface.addressEntries();
		for (const QNetworkAddressEntry& addressEntry : entries)
		{
			if (isValidAddressEntry(addressEntry))
			{
				broadcastEntries << addressEntry;
			}
		}
	}

	return broadcastEntries;
}


void DatagramHandlerImpl::send(const QByteArray& pData, const QList<QNetworkAddressEntry>& pEntries)
{
	sendToAddressEntries(pData, pEntries, 0);
}


bool DatagramHandlerImpl::isValidBroadcastInterface(const QNetworkInterface& pInterface) const
{
	if (!pInterface.isValid())
	{
		return false;
	}

	const auto& flags = pInterface.flags();
	if (!flags.testFlag(QNetworkInterface::IsUp) || !flags.testFlag(QNetworkInterface::IsRunning))
	{
		return false;
	}

#ifdef Q_OS_MACOS
	// Excluding not documented interface of the T2 Coprocessor on macOS,  which does not accept broadcasts.
	// https://duo.com/labs/research/apple-t2-xpc
	if (pInterface.hardwareAddress().toLower() == QLatin1String("ac:de:48:00:11:22"))
	{
		return false;
	}
#endif

	return true;
}


bool DatagramHandlerImpl::isValidAddressEntry(const QNetworkAddressEntry& pEntry) const
{
	const auto ipAddr = pEntry.ip();
	switch (ipAddr.protocol())
	{
		case QAbstractSocket::NetworkLayerProtocol::IPv4Protocol:
			return ipAddr.isGlobal() && !pEntry.broadcast().isNull();

		case QAbstractSocket::NetworkLayerProtocol::IPv6Protocol:
			return ipAddr.isGlobal();

		default:
			qCDebug(network) << "Skipping unknown protocol type:" << ipAddr.protocol();
			return false;

	}
}


QHostAddress DatagramHandlerImpl::getBroadcastAddress(const QNetworkAddressEntry& pEntry) const
{
	const auto& ipAddr = pEntry.ip();
	switch (ipAddr.protocol())
	{
		case QAbstractSocket::NetworkLayerProtocol::IPv4Protocol:
		{
			return pEntry.broadcast();
		}

		case QAbstractSocket::NetworkLayerProtocol::IPv6Protocol:
		{
			return QHostAddress(ipv6MulticastAddress);
		}

		default:
		{
			qCDebug(network) << "Skipping unknown protocol type:" << ipAddr.protocol();
			return QHostAddress();
		}
	}
}


bool DatagramHandlerImpl::sendToAddress(const QByteArray& pData, const QHostAddress& pAddress, quint16 pPort, bool pLogError)
{
	// If port is 0 we should take our own listening port as destination as other instances
	// should use the same port to receive broadcasts.
	const auto port = pPort > 0 ? pPort : mUsedPort;
	Q_ASSERT(port > 0);

	if (mSocket->writeDatagram(pData.constData(), pAddress, port) != pData.size())
	{
		if (pLogError)
		{
			qCCritical(network) << "Cannot write datagram to address" << pAddress << ':' << mSocket->error() << '|' << mSocket->errorString();
		}
		return false;
	}

	return true;
}


void DatagramHandlerImpl::sendToAddressEntries(const QByteArray& pData, const QList<QNetworkAddressEntry>& pEntries, quint16 pPort)
{
	// Check if something changed because We don't want
	// to log all interfaces for any broadcast here.
	if (mAllEntries != pEntries)
	{
		mAllEntries = pEntries;
		mFailedAddresses.clear();
		qCDebug(network) << "Broadcast Addresses changed...";
		for (const auto& broadcastEntry : pEntries)
		{
			qCDebug(network) << broadcastEntry;
		}
	}

	if (pEntries.isEmpty())
	{
		return;
	}

	QList<QHostAddress> alreadySent;
	for (const auto& broadcastEntry : pEntries)
	{
		const auto& address = getBroadcastAddress(broadcastEntry);
		if (alreadySent.contains(address))
		{
			qCDebug(network) << "Skipping duplicate broadcasting to" << address;
			continue;
		}

		const auto alreadyFailed = mFailedAddresses.contains(address);
		const auto sendSuccess = sendToAddress(pData, address, pPort, !alreadyFailed);
		if (sendSuccess)
		{
			alreadySent << address;
			if (alreadyFailed)
			{
				mFailedAddresses.removeAll(address);
			}
			continue;
		}

		if (!alreadyFailed)
		{
			qCDebug(network) << "Broadcasting to" << address << "failed";
			mFailedAddresses << address;
		}
	}
}


#if defined(Q_OS_IOS)
void DatagramHandlerImpl::checkNetworkPermission()
{
	// The permission prompt is only triggered when sending data.
	// So send a dummy packet, as that will hopefully trigger the prompt.
	// Use port 9 as all traffic send to it is discarded (RFC863).

	quint16 discardServicePort = 9;
	if (!sendToAddress(QByteArrayLiteral("!"), QHostAddress::LocalHost, discardServicePort)
			|| !sendToAddress(QByteArrayLiteral("!"), QHostAddress::LocalHostIPv6, discardServicePort))
	{
		qCDebug(network) << "Probably no permission to access the local network, resetting broadcasting socket.";
		resetSocket();
	}
}


#endif


void DatagramHandlerImpl::onReadyRead()
{
	while (mSocket->hasPendingDatagrams())
	{
		const QNetworkDatagram& datagram = mSocket->receiveDatagram();
		if (!datagram.isValid())
		{
			qCCritical(network) << "Cannot read datagram";
			Q_ASSERT(false);
			continue;
		}
		Q_EMIT fireNewMessage(datagram.data(), datagram.senderAddress());
	}
}
