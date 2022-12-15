/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "DiagnosisController.h"

#include "ReaderDetector.h"
#include "ReaderManager.h"

#include <QLoggingCategory>
#include <QNetworkInterface>
#include <QSysInfo>
#include <QtConcurrent>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(diagnosis)

DiagnosisController::DiagnosisController(const QSharedPointer<DiagnosisContext>& pContext, QObject* pParent)
	: QObject(pParent)
	, mContext(pContext)
	, mWatcherPcscInfo()
	, mScanHasToBeStopped(false)
{
	connect(&mWatcherPcscInfo, &QFutureWatcher<PcscInfo>::finished, this, &DiagnosisController::onPcscInfoRetrieved);

	connect(Env::getSingleton<ReaderDetector>(), &ReaderDetector::fireReaderChangeDetected, this, &DiagnosisController::onReaderEvent);

	const auto& readerManager = Env::getSingleton<ReaderManager>();
	connect(readerManager, &ReaderManager::fireReaderAdded, this, &DiagnosisController::onReaderEvent);
	connect(readerManager, &ReaderManager::fireReaderRemoved, this, &DiagnosisController::onReaderEvent);
	connect(readerManager, &ReaderManager::fireCardInserted, this, &DiagnosisController::onReaderEvent);
	connect(readerManager, &ReaderManager::fireCardRemoved, this, &DiagnosisController::onReaderEvent);
	connect(readerManager, &ReaderManager::fireReaderPropertiesUpdated, this, &DiagnosisController::onReaderEvent);
}


DiagnosisController::~DiagnosisController()
{
	if (mScanHasToBeStopped)
	{
		const auto& readerManager = Env::getSingleton<ReaderManager>();
		if (readerManager->isScanRunning(ReaderManagerPlugInType::PCSC))
		{
			qCDebug(diagnosis) << "Stopping PCSC scan.";
			readerManager->stopScan(ReaderManagerPlugInType::PCSC);
		}
		mScanHasToBeStopped = false;
	}
}


void DiagnosisController::run()
{
	const auto& readerManager = Env::getSingleton<ReaderManager>();
	if (!readerManager->isScanRunning(ReaderManagerPlugInType::PCSC))
	{
		qCDebug(diagnosis) << "PCSC scan not running, starting scan ourself and stop it afterwards.";
		readerManager->startScan(ReaderManagerPlugInType::PCSC);
		mScanHasToBeStopped = true;
	}

	mWatcherPcscInfo.setFuture(QtConcurrent::run(&DiagnosisController::retrievePcscInfo));
	collectInterfaceInformation();
}


void DiagnosisController::onPcscInfoRetrieved()
{
	auto info = mWatcherPcscInfo.future().result();
	mContext->setPcscInfo(info.mPcscVersion, info.mPcscComponents, info.mPcscDrivers);
	onReaderEvent();
}


void DiagnosisController::collectInterfaceInformation()
{
	mContext->setNetworkInterfaces(QNetworkInterface::allInterfaces());
}


DiagnosisController::PcscInfo DiagnosisController::retrievePcscInfo()
{
	PcscInfo result;
	const auto& pcscInfo = Env::getSingleton<ReaderManager>()->getPlugInInfo(ReaderManagerPlugInType::PCSC);
	if (pcscInfo.isAvailable())
	{
		const QVariant version = pcscInfo.getValue(ReaderManagerPlugInInfo::Key::PCSC_LITE_VERSION);
		if (version.isValid())
		{
			result.mPcscVersion = tr("pcsclite %1").arg(version.toString());
		}
		else
		{
			tr("unknown");
		}
	}
	else
	{
		result.mPcscVersion = tr("not available");
	}

	getPcscInfo(result.mPcscComponents, result.mPcscDrivers);
	if (!result.mPcscComponents.isEmpty())
	{
		result.mPcscVersion = result.mPcscComponents.first().getVersion();
	}

	return result;
}


void DiagnosisController::onReaderEvent()
{
	const auto& readersWithDriver = Env::getSingleton<ReaderManager>()->getReaderInfos();
	QVector<ReaderConfigurationInfo> readersWithDriverInfos;
	for (const auto& reader : readersWithDriver)
	{
		readersWithDriverInfos << reader.getReaderConfigurationInfo();
	}

	auto attachedDevices = Env::getSingleton<ReaderDetector>()->getAttachedSupportedDevices();
	const auto& driverIsInstalled =
			[readersWithDriverInfos](const ReaderConfigurationInfo& readerInfo)
			{
				return readersWithDriverInfos.contains(readerInfo);
			};

	attachedDevices.erase(
			std::remove_if(attachedDevices.begin(), attachedDevices.end(), driverIsInstalled),
			attachedDevices.end());

	mContext->setReaderInfosNoDriver(attachedDevices);

	mContext->setReaderInfos(readersWithDriver);
	mContext->setTimestamp(QDateTime::currentDateTime());
}
