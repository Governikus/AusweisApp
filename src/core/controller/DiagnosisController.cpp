/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "DiagnosisController.h"

#include "ReaderManager.h"

#include <QNetworkInterface>
#include <QSysInfo>
#include <QtConcurrent/QtConcurrent>

using namespace governikus;


DiagnosisController::DiagnosisController(const QSharedPointer<DiagnosisContext>& pContext, QObject* pParent)
	: QObject(pParent)
	, mContext(pContext)
	, mWatcherPcscInfo()
{
	connect(&mWatcherPcscInfo, &QFutureWatcher<PcscInfo>::finished, this, &DiagnosisController::onPcscInfoRetrieved);
}


DiagnosisController::~DiagnosisController()
{
}


void DiagnosisController::run()
{
	mWatcherPcscInfo.setFuture(QtConcurrent::run(&DiagnosisController::retrievePcscInfo));
	collectInterfaceInformation();
}


void DiagnosisController::onPcscInfoRetrieved()
{
	auto info = mWatcherPcscInfo.future().result();
	mContext->setPcscInfo(info.mPcscVersion, info.mPcscComponents, info.mPcscDrivers);
	checkDone();
}


void DiagnosisController::checkDone()
{
	if (mWatcherPcscInfo.isFinished())
	{
		mContext->setReaderInfos(Env::getSingleton<ReaderManager>()->getReaderInfos());
		mContext->setTimestamp(QDateTime::currentDateTime());
	}
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
