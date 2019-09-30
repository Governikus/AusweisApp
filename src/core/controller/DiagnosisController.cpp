/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "DiagnosisController.h"

#include "ReaderManager.h"

#include <QLoggingCategory>
#include <QNetworkInterface>
#include <QSysInfo>
#include <QtConcurrent/QtConcurrent>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(diagnosis)

DiagnosisController::DiagnosisController(const QSharedPointer<DiagnosisContext>& pContext, QObject* pParent)
	: QObject(pParent)
	, mContext(pContext)
	, mWatcherPcscInfo()
	, mScanHasToBeStopped(false)
{
	connect(&mWatcherPcscInfo, &QFutureWatcher<PcscInfo>::finished, this, &DiagnosisController::onPcscInfoRetrieved);

	const auto& readerManager = Env::getSingleton<ReaderManager>();
	connect(readerManager, &ReaderManager::fireReaderEvent, this, &DiagnosisController::onReaderEvent);
}


DiagnosisController::~DiagnosisController()
{
	if (mScanHasToBeStopped)
	{
		qCDebug(diagnosis) << "Stopping scans.";
		const auto& readerManager = Env::getSingleton<ReaderManager>();
		readerManager->stopScanAll();
		mScanHasToBeStopped = false;
	}
}


void DiagnosisController::run()
{
	const auto& readerManager = Env::getSingleton<ReaderManager>();
	if (!readerManager->isScanRunning())
	{
		qCDebug(diagnosis) << "Scan not running, starting scan ourself and stop it afterwards.";
		readerManager->startScanAll(false);
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

	bool hasPcsc = false;
	const auto infos = Env::getSingleton<ReaderManager>()->getPlugInInfos();
	for (const auto& info : infos)
	{
		if (info.getPlugInType() == ReaderManagerPlugInType::PCSC)
		{
			hasPcsc = true;

			QVariant version = info.getValue(ReaderManagerPlugInInfo::Key::PCSC_LITE_VERSION);
			if (version.isValid())
			{
				result.mPcscVersion = tr("pcsclite %1").arg(version.toString());
			}
		}
	}

	if (result.mPcscVersion.isEmpty())
	{
		result.mPcscVersion = hasPcsc ? tr("unknown") : tr("not available");
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
	mContext->setReaderInfos(Env::getSingleton<ReaderManager>()->getReaderInfos());
	mContext->setTimestamp(QDateTime::currentDateTime());
}
