/*!
 * \copyright Copyright (c) 2022 Governikus GmbH & Co. KG, Germany
 */

#include "PortWrapper.h"

#include "PortFile.h"

#include <QFile>
#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(rproxy)


PortWrapper::PortWrapper(const QSharedPointer<HttpRequest>& pRequest)
	: mPortFiles(PortFile::getAllPortFiles())
{
	Q_UNUSED(pRequest)
}


bool PortWrapper::isEmpty() const
{
	return mPortFiles.isEmpty();
}


void PortWrapper::invalidate()
{
	mPortFiles.removeFirst();
}


quint16 PortWrapper::readPortFile(const QString& pFile)
{
	QFile portfile(pFile);
	if (portfile.exists() && portfile.open(QIODevice::ReadOnly | QIODevice::Unbuffered))
	{
		return static_cast<quint16>(portfile.readAll().toInt());
	}

	return 0;
}


quint16 PortWrapper::fetchPort()
{
	for (QMutableListIterator iter(mPortFiles); iter.hasNext();)
	{
		const auto& filename = iter.next().absoluteFilePath();
		const auto port = readPortFile(filename);
		if (port > 0)
		{
			return port;
		}

		qCWarning(rproxy) << "Ignore invalid port file:" << filename;
		iter.remove();
	}

	return 0;
}
