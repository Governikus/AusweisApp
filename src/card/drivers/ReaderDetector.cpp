/*!
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderDetector.h"

#include "Env.h"
#include "FuncUtils.h"
#include "ReaderConfiguration.h"
#include "SingletonHelper.h"

#include <QFile>
#include <QLoggingCategory>
#include <QRegularExpression>
#include <QScopedPointer>


using namespace governikus;


defineSingleton(ReaderDetector)


Q_DECLARE_LOGGING_CATEGORY(card_drivers)


ReaderDetector & ReaderDetector::getInstance()
{
	return *Instance;
}


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


QVector<ReaderConfigurationInfo> ReaderDetector::getAttachedSupportedDevices() const
{
	const auto& readerConfiguration = Env::getSingleton<ReaderConfiguration>();
	QVector<ReaderConfigurationInfo> attachedSupportedDevices;

	const auto& devIds = attachedDevIds();
	for (const auto& devId : devIds)
	{
		const auto& readerConfigurationInfo = readerConfiguration->getReaderConfigurationInfoById(devId);
		if (readerConfigurationInfo.isKnownReader() && !readerConfigurationInfo.getUrl().isEmpty())
		{
			qCDebug(card_drivers) << "Found known reader:" << devId;
			attachedSupportedDevices += readerConfigurationInfo;
		}
	}
	return attachedSupportedDevices;
}


ReaderConfigurationInfo ReaderDetector::getReaderConfigurationInfo(const QString& pReaderName)
{
	QVector<ReaderConfigurationInfo> attachedSupportedDevices = getAttachedSupportedDevices();
	attachedSupportedDevices += Env::getSingleton<ReaderConfiguration>()->getRemoteReaderConfigurationInfo();

	for (const auto& info : qAsConst(attachedSupportedDevices))
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
