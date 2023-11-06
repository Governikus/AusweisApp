/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Controller for retrieving and presenting diagnosis info.
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

	private:
		struct PcscInfo
		{
			QString mPcscVersion;
			QVector<DiagnosisContext::ComponentInfo> mPcscComponents;
			QVector<DiagnosisContext::ComponentInfo> mPcscDrivers;
		};

		QSharedPointer<DiagnosisContext> mContext;
		QFutureWatcher<PcscInfo> mWatcherPcscInfo;

		void collectInterfaceInformation();

		static PcscInfo retrievePcscInfo();
		static void getPcscInfo(QVector<DiagnosisContext::ComponentInfo>& pComponents,
				QVector<DiagnosisContext::ComponentInfo>& pDrivers);

	public:
		explicit DiagnosisController(const QSharedPointer<DiagnosisContext>& pContext, QObject* pParent = nullptr);
		~DiagnosisController() override;

		void run();

	private Q_SLOTS:
		void onPcscInfoRetrieved();
		void onReaderEvent();


};


} // namespace governikus
