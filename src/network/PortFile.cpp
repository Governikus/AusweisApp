/**
 * Copyright (c) 2018-2026 Governikus Service GmbH, Germany
 */

#include "PortFile.h"

#include <QDir>
#include <QLoggingCategory>
#include <QStringBuilder>

Q_DECLARE_LOGGING_CATEGORY(network)

using namespace governikus;

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


PortFile::~PortFile()
{
	remove();
}


void PortFile::handlePort(quint16 pCurrentPort)
{
	if (pCurrentPort != mDefaultPort && mPortFile.open(QIODevice::WriteOnly) && mPortFile.isWritable())
	{
		mPortFile.write(QByteArray::number(pCurrentPort));
		mPortFile.close();
	}
}


void PortFile::remove()
{
	if (mPortFile.exists())
	{
		mPortFile.remove();
	}
}


const QFile& PortFile::getFile() const
{
	return mPortFile;
}


QFileInfoList PortFile::getAllPortFiles()
{
	QDir tmpPath = QDir::temp();
	tmpPath.setSorting(QDir::Time);
	tmpPath.setFilter(QDir::Files);
	tmpPath.setNameFilters(QStringList({QCoreApplication::applicationName() + QStringLiteral(".*.port")}));
	return tmpPath.entryInfoList();
}


QList<quint16> PortFile::readAllPortFiles()
{
	QList<quint16> ports;

	const auto& portFiles = getAllPortFiles();
	for (const auto& portFile : portFiles)
	{
		const auto& filename = portFile.absoluteFilePath();
		const auto port = readPortFile(filename);

		if (port < 1)
		{
			qCWarning(network) << "Ignore invalid port file:" << filename;
			continue;
		}

		ports << port;
	}

	return ports;
}


quint16 PortFile::readPortFile(const QString& pFile)
{
	QFile portfile(pFile);
	if (portfile.exists() && portfile.open(QIODevice::ReadOnly | QIODevice::Unbuffered))
	{
		return static_cast<quint16>(portfile.readAll().toInt());
	}

	return 0;
}
