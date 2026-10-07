/**
 * Copyright (c) 2022-2026 Governikus Service GmbH, Germany
 */

#include "PortWrapper.h"

#include "PortFile.h"

#include <QFile>


using namespace governikus;


QList<quint16> PortWrapper::fetchPorts(quint16 pLocalPort, quint16 pPeerPort)
{
	Q_UNUSED(pPeerPort)

	auto ports = PortFile::readAllPortFiles();
	ports.removeAll(pLocalPort);
	return ports;
}
