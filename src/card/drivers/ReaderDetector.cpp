/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderDetector.h"

#include "ReaderConfiguration.h"

#include <QFile>
#include <QLoggingCategory>
#include <QRegularExpression>
#include <QScopedPointer>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(card_drivers)


ReaderDetector::ReaderDetector()
#ifdef Q_OS_LINUX
	: mDeviceListener(nullptr)
#endif
{
	qCDebug(card_drivers) << "initNativeEvents() =" << initNativeEvents();
}


ReaderDetector::~ReaderDetector()
{
	qCDebug(card_drivers) << "terminateNativeEvents() =" << terminateNativeEvents();
}


QList<ReaderConfigurationInfo> ReaderDetector::getAttachedSupportedDevices() const
{
	const auto& readerConfiguration = Env::getSingleton<ReaderConfiguration>();
	QList<ReaderConfigurationInfo> attachedSupportedDevices;

	const auto& devIds = attachedDevIds();
	for (const auto& devId : devIds)
	{
		const auto& readerConfigurationInfo = readerConfiguration->getReaderConfigurationInfoById(devId);
		if (readerConfigurationInfo.isKnownReader() && !readerConfigurationInfo.getUrl().isEmpty())
		{
			attachedSupportedDevices += readerConfigurationInfo;
		}
	}
	return attachedSupportedDevices;
}


ReaderConfigurationInfo ReaderDetector::getReaderConfigurationInfo(const QString& pReaderName) const
{
	QList<ReaderConfigurationInfo> attachedSupportedDevices = getAttachedSupportedDevices();
	attachedSupportedDevices += Env::getSingleton<ReaderConfiguration>()->getVirtualReaderConfigurationInfos();

	for (const auto& info : std::as_const(attachedSupportedDevices))
	{
		const QString& pattern = info.getPattern();
		const QRegularExpression expression(pattern.isEmpty() ? info.getName() : pattern);
		if (pReaderName.contains(expression))
		{
			return info;
		}
	}

	return ReaderConfigurationInfo(pReaderName);
}
