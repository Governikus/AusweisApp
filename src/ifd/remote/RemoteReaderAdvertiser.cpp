/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
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

template<> RemoteReaderAdvertiser* createNewObject<RemoteReaderAdvertiser*, const QString&, const QString&, quint16&, bool&>(const QString& pIfdName, const QString& pIfdId, quint16& pPort, bool& pIsPairing)
{
	return new RemoteReaderAdvertiserImpl(pIfdName, pIfdId, pPort, pIsPairing);
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


RemoteReaderAdvertiserImpl::RemoteReaderAdvertiserImpl(const QString& pIfdName, const QString& pIfdId, quint16 pPort, bool pPairing, int pTimerInterval)
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
