/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteReaderAdvertiser.h"

#include "DatagramHandler.h"
#include "Env.h"
#include "messages/Discovery.h"

#include <QLoggingCategory>
#include <QTimerEvent>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


namespace governikus
{

template<> RemoteReaderAdvertiser* createNewObject<RemoteReaderAdvertiser*, const QString&, const QString&, quint16&>(const QString& pIfdName, const QString& pIfdId, quint16& pPort)
{
	return new RemoteReaderAdvertiserImpl(pIfdName, pIfdId, pPort);
}


template<> RemoteReaderAdvertiser* createNewObject<RemoteReaderAdvertiser*, const QString&, const QString&, quint16&, int&>(const QString& pIfdName, const QString& pIfdId, quint16& pPort, int& pTimerInterval)
{
	return new RemoteReaderAdvertiserImpl(pIfdName, pIfdId, pPort, pTimerInterval);
}


} // namespace governikus


using namespace governikus;


RemoteReaderAdvertiser::~RemoteReaderAdvertiser()
{
}


void RemoteReaderAdvertiserImpl::timerEvent(QTimerEvent* pEvent)
{
	if (pEvent->timerId() == mTimerId)
	{
		mHandler->send(mDiscovery.toByteArray(IfdVersion::Version::latest));
	}
}


RemoteReaderAdvertiserImpl::~RemoteReaderAdvertiserImpl()
{
	qCDebug(remote_device) << "Stop advertising";
}


RemoteReaderAdvertiserImpl::RemoteReaderAdvertiserImpl(const QString& pIfdName, const QString& pIfdId, quint16 pPort, int pTimerInterval)
	: RemoteReaderAdvertiser()
	, mHandler(Env::create<DatagramHandler*>(false))
	, mTimerId(startTimer(pTimerInterval))
	, mDiscovery(Discovery(pIfdName, pIfdId, pPort, {IfdVersion::supported()}))
{
	qCDebug(remote_device) << "Start advertising every" << pTimerInterval << "msecs";
}


void RemoteReaderAdvertiserImpl::setPairing(bool pEnabled)
{
	mDiscovery.setPairing(pEnabled);
}
