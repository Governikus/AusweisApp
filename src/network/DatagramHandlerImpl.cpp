/*!
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "DatagramHandlerImpl.h"

#include "Env.h"

#include <QLoggingCategory>
#include <QMutexLocker>
#include <QNetworkDatagram>
#include <QNetworkInterface>
#include <QNetworkProxy>
#include <QOperatingSystemVersion>
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
	, mAllAddresses()
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


void DatagramHandlerImpl::send(const QByteArray& pData)
{
	sendToAllAddressEntries(pData, 0);
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


QVector<QHostAddress> DatagramHandlerImpl::getAllBroadcastAddresses(const QNetworkInterface& pInterface) const
{
	QVector<QHostAddress> broadcastAddresses;

	if (!isValidBroadcastInterface(pInterface))
	{
		return broadcastAddresses;
	}

	bool skipFurtherIPv6AddressesOnThisInterface = false;

	const auto& entries = pInterface.addressEntries();
	for (const QNetworkAddressEntry& addressEntry : entries)
	{
		const auto ipAddr = addressEntry.ip();
		switch (ipAddr.protocol())
		{
			case QAbstractSocket::NetworkLayerProtocol::IPv4Protocol:
			{
				const QHostAddress& broadcastAddr = addressEntry.broadcast();
				if (broadcastAddr.isNull() || !ipAddr.isGlobal())
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

				const QString& scopeId = ipAddr.scopeId();
				if (scopeId.isEmpty() || !(ipAddr.isLinkLocal() || ipAddr.isUniqueLocalUnicast()))
				{
					continue;
				}

				QHostAddress scopedMulticastAddress(QStringLiteral("ff02::1"));
				scopedMulticastAddress.setScopeId(scopeId);
				broadcastAddresses += scopedMulticastAddress;

				skipFurtherIPv6AddressesOnThisInterface = true;
				break;
			}

			default:
			{
				qCDebug(network) << "Skipping unknown protocol type:" << ipAddr.protocol();
			}
		}
	}

	return broadcastAddresses;
}


void DatagramHandlerImpl::sendToAllAddressEntries(const QByteArray& pData, quint16 pPort)
{
	const auto& allInterfaces = QNetworkInterface::allInterfaces();

	// QNetworkInterface has no operator== so we use allAddresses()
	// to check if something changed. We don't want to log
	// all interfaces for any broadcast here.
	const auto& addresses = QNetworkInterface::allAddresses();
	if (mAllAddresses != addresses)
	{
		mAllAddresses = addresses;
		mFailedAddresses.clear();
		qCDebug(network) << "Network interfaces changed...";
		for (const auto& interface : allInterfaces)
		{
			qCDebug(network) << interface;
		}
	}

	QVector<QHostAddress> broadcastAddresses;
	for (const QNetworkInterface& interface : allInterfaces)
	{
		broadcastAddresses << getAllBroadcastAddresses(interface);
	}

	if (broadcastAddresses.isEmpty())
	{
		return;
	}

	for (const QHostAddress& broadcastAddr : std::as_const(broadcastAddresses))
	{
		const auto alreadyFailed = mFailedAddresses.contains(broadcastAddr);
		const auto sendSuccess = sendToAddress(pData, broadcastAddr, pPort, !alreadyFailed);

		if (sendSuccess && alreadyFailed)
		{
			mFailedAddresses.removeAll(broadcastAddr);
		}
		else if (!sendSuccess && !alreadyFailed)
		{
			qCDebug(network) << "Broadcasting to" << broadcastAddr << "failed";
			mFailedAddresses << broadcastAddr;
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


#if defined(Q_OS_IOS)
void DatagramHandlerImpl::checkNetworkPermission()
{
	if (QOperatingSystemVersion::current() < QOperatingSystemVersion(QOperatingSystemVersion::IOS, 14))
	{
		return;
	}

	// iOS 14 doesn't have an API to ask for local network permissions,
	// furthermore the permission prompt is only triggered when sending data.
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
