/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteReaderAdvertiser.h"

#include "DatagramHandler.h"
#include "Env.h"
#include "messages/Discovery.h"

#include <QLoggingCategory>
#include <QTimerEvent>


Q_DECLARE_LOGGING_CATEGORY(ifd)


namespace governikus
{

template<> RemoteReaderAdvertiser* createNewObject<RemoteReaderAdvertiser*, const QString&, const QString&, quint16&>(const QString& pIfdName, const QString& pIfdId, quint16& pPort)
{
	return new RemoteReaderAdvertiserImpl(pIfdName, pIfdId, pPort);
}


} // namespace governikus


using namespace governikus;


void RemoteReaderAdvertiserImpl::timerEvent(QTimerEvent* pEvent)
{
	if (pEvent->timerId() == mTimerId)
	{
		sendDiscovery();
	}
}


void RemoteReaderAdvertiserImpl::sendDiscovery()
{
	mHandler->send(mDiscovery.toByteArray(IfdVersion::Version::latest));
}


RemoteReaderAdvertiserImpl::~RemoteReaderAdvertiserImpl()
{
	qCDebug(ifd) << "Stop advertising";
}


RemoteReaderAdvertiserImpl::RemoteReaderAdvertiserImpl(const QString& pIfdName, const QString& pIfdId, quint16 pPort, int pTimerInterval, bool pPairing)
	: RemoteReaderAdvertiser()
	, mHandler(Env::create<DatagramHandler*>(false))
	, mTimerId(startTimer(pTimerInterval))
	, mDiscovery(Discovery(pIfdName, pIfdId, pPort, {IfdVersion::supported()}, pPairing))
{
	qCDebug(ifd) << "Start advertising every" << pTimerInterval << "msecs";
	sendDiscovery();
}


void RemoteReaderAdvertiserImpl::setPairing(bool pEnabled)
{
	mDiscovery.setPairing(pEnabled);
}
