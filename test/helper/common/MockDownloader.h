/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief MockDownloader of class Downloader, part of \ref Updater and \ref UpdatableFile tests
 */


#pragma once

#include "Downloader.h"
#include "GlobalStatus.h"

#include <QMap>

namespace governikus
{
class MockDownloader
	: public Downloader
{
	Q_OBJECT

	private:
		GlobalStatus::Code mErrorCode;
		QDate mDate;
		QTime mTime;
		QMap<QUrl, QByteArray> mTestData;

	public:
		MockDownloader(GlobalStatus::Code pErrorCode = GlobalStatus::Code::No_Error);
		~MockDownloader() override = default;

		QDateTime getTimeStamp();
		QString getTimeStampString();
		void setTestData(QUrl& pUrl, const QByteArray& pData);
		QByteArray getTestData(const QUrl& pUrl);
		void setError(GlobalStatus::Code pErrorCode);
		void download(const QUrl& pUpdateUrl, const QDateTime& pCurrentTimestamp = QDateTime()) override;
};

} // namespace governikus
