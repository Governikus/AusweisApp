/*!
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

#include "PortFile.h"

#include <QDir>
#include <QStringBuilder>

using namespace governikus;

const quint16 PortFile::cDefaultPort = 24727;

QString PortFile::getPortFilename(const QString& pUsage, qint64 pPid, const QString& pApp)
{
	const QLatin1Char sep('.');
	const auto& usage = pUsage.isEmpty() ? pUsage : sep % pUsage;

	return QDir::tempPath() % QDir::separator() % pApp % sep % QString::number(pPid) % usage % QStringLiteral(".port");
}


PortFile::PortFile(const QString& pUsage, quint16 pDefaultPort)
	: mDefaultPort(pDefaultPort)
	, mPortFile(getPortFilename(pUsage))
{
}


void PortFile::handlePort(quint16 pCurrentPort)
{
	if (pCurrentPort != mDefaultPort && mPortFile.open(QIODevice::WriteOnly) && mPortFile.isWritable())
	{
		mPortFile.write(QByteArray::number(pCurrentPort));
		mPortFile.close();
	}
}


PortFile::~PortFile()
{
	if (mPortFile.exists())
	{
		mPortFile.remove();
	}
}
