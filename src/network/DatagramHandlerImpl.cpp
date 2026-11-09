/**
 * Copyright (c) 2016-2026 Governikus Service GmbH, Germany
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
	, mFailedMulticastInterfaces()
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
		bool joinedMulticast = false;
		const auto& allInterfaces = QNetworkInterface::allInterfaces();
		for (const auto& iface : allInterfaces)
		{
			if (!isValidBroadcastInterface(iface) || !iface.flags().testFlag(QNetworkInterface::CanMulticast))
			{
				continue;
			}

			bool hasIpv6 = false;
			for (const auto& entry : iface.addressEntries())
			{
				if (entry.ip().protocol() == QAbstractSocket::IPv6Protocol)
				{
					hasIpv6 = true;
					break;
				}
			}
			if (!hasIpv6)
			{
				continue;
			}

			if (mSocket->joinMulticastGroup(QHostAddress(ipv6MulticastAddress), iface))
			{
				joinedMulticast = true;
			}
			else
			{
				qCDebug(network) << "Could not join multicast group on interface" << iface.name() << ':' << mSocket->errorString();
			}
		}

		if (!joinedMulticast && !mSocket->joinMulticastGroup(QHostAddress(ipv6MulticastAddress)))
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


void DatagramHandlerImpl::send(const QByteArray& pData, const QList<QNetworkAddressEntry>& pEntries)
{
	sendToAddressEntries(pData, pEntries, 0);
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
	// Check if something changed because we don't want
	// to log all interfaces for any broadcast here.
	if (mAllEntries != pEntries)
	{
		mAllEntries = pEntries;
		mFailedAddresses.clear();
		mFailedMulticastInterfaces.clear();
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
	QSet<int> alreadySentMulticastInterfaces;
	const auto& allInterfaces = QNetworkInterface::allInterfaces();
	for (const auto& broadcastEntry : pEntries)
	{
		const auto& address = getBroadcastAddress(broadcastEntry);
		if (address.isNull())
		{
			continue;
		}

		QNetworkInterface targetInterface;
		if (address.protocol() == QAbstractSocket::IPv6Protocol && address.isMulticast())
		{
			for (const auto& iface : allInterfaces)
			{
				if (!isValidBroadcastInterface(iface) || !iface.flags().testFlag(QNetworkInterface::CanMulticast))
				{
					continue;
				}

				for (const auto& entry : iface.addressEntries())
				{
					if (entry.ip() == broadcastEntry.ip())
					{
						targetInterface = iface;
						break;
					}
				}

				if (targetInterface.isValid())
				{
					break;
				}
			}

			if (targetInterface.isValid())
			{
				if (alreadySentMulticastInterfaces.contains(targetInterface.index()))
				{
					continue;
				}
				alreadySentMulticastInterfaces << targetInterface.index();

				if (!isBound())
				{
					if (!mSocket->bind(QHostAddress::Any, 0))
					{
						qCDebug(network) << "Cannot bind socket for multicast:" << mSocket->errorString();
					}
				}
				mSocket->setMulticastInterface(targetInterface);
			}
			else if (alreadySent.contains(address))
			{
				continue;
			}
		}
		else if (alreadySent.contains(address))
		{
			continue;
		}

		const bool isMulticastOnInterface = targetInterface.isValid();
		const auto alreadyFailed = isMulticastOnInterface ? mFailedMulticastInterfaces.contains(targetInterface.index())
			: mFailedAddresses.contains(address);
		const auto sendSuccess = sendToAddress(pData, address, pPort, !alreadyFailed);
		if (sendSuccess)
		{
			alreadySent << address;
			if (alreadyFailed)
			{
				if (isMulticastOnInterface)
				{
					mFailedMulticastInterfaces.remove(targetInterface.index());
				}
				else
				{
					mFailedAddresses.removeAll(address);
				}
			}
			continue;
		}

		if (!alreadyFailed)
		{
			if (isMulticastOnInterface)
			{
				qCDebug(network) << "Broadcasting to" << address << "on interface" << targetInterface.name() << "failed";
				mFailedMulticastInterfaces.insert(targetInterface.index());
			}
			else
			{
				qCDebug(network) << "Broadcasting to" << address << "failed";
				mFailedAddresses << address;
			}
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
