/*!
 * \brief Controller for retrieving and presenting diagnosis info.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QDebug>
#include <QFutureWatcher>
#include <QScopedPointer>

#include "context/DiagnosisContext.h"

namespace governikus
{

class DiagnosisController
	: public QObject
{
	Q_OBJECT

	struct PcscInfo
	{
		QString mPcscVersion;
		QVector<DiagnosisContext::ComponentInfo> mPcscComponents;
		QVector<DiagnosisContext::ComponentInfo> mPcscDrivers;
	};

	private:
		QScopedPointer<DiagnosisContext> mContext;
		QFutureWatcher<PcscInfo> mWatcherPcscInfo;

		void checkDone();

		static PcscInfo retrievePcscInfo();
		static void getPcscInfo(QVector<DiagnosisContext::ComponentInfo>& pComponents,
				QVector<DiagnosisContext::ComponentInfo>& pDrivers);

	public:
		DiagnosisController(DiagnosisContext* pContext, QObject* pParent = nullptr);
		virtual ~DiagnosisController();

		void run();

	private Q_SLOTS:
		void onPcscInfoRetrieved();


};


} /* namespace governikus */
