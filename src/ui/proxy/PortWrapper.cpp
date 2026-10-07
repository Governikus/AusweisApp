/**
 * Copyright (c) 2022-2026 Governikus Service GmbH, Germany
 */

#include "PortWrapper.h"

#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(rproxy)


PortWrapper::PortWrapper(const QList<quint16>& pPorts)
	: mPorts(pPorts)
{
	qCDebug(rproxy) << "Found instances on Ports:" << mPorts;
}


PortWrapper::PortWrapper(quint16 pLocalPort, quint16 pPeerPort)
	: PortWrapper(fetchPorts(pLocalPort, pPeerPort))
{
}


bool PortWrapper::isEmpty() const
{
	return mPorts.isEmpty();
}


quint16 PortWrapper::pop()
{
	if (isEmpty())
	{
		return 0;
	}

	return mPorts.takeFirst();
}
