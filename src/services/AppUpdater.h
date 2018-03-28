/*!
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AppUpdateData.h"
#include "GlobalStatus.h"

#include <QDateTime>
#include <QUrl>

namespace governikus
{
class AppUpdater
	: public QObject
{
	Q_OBJECT

	private:
		bool mIgnoreNextVersionskip;
		QUrl mAppUpdateJsonUrl;
		AppUpdateData mAppUpdateData;

		void clearDownloaderConnection();

	public:
		AppUpdater();
		virtual ~AppUpdater() = default;
		static AppUpdater& getInstance();

		void checkAppUpdate(bool pIgnoreNextVersionskip = false);
		const AppUpdateData& getUpdateData() const;
		void skipVersion(const QString& pVersion);

	private Q_SLOTS:
		void onUpdateDownloadFinished(const QUrl& pUpdateUrl, const QDateTime& pNewTimestamp, const QByteArray& pData);
		void onUpdateDownloadFailed(const QUrl& pUpdateUrl, GlobalStatus::Code pErrorCode);
		void onUpdateDownloadUnnecessary(const QUrl& pUpdateUrl);

	Q_SIGNALS:
		void fireAppUpdateCheckFinished(bool pUpdateAvailable, const GlobalStatus& pError);
};

} /* namespace governikus */
