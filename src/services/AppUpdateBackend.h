/*!
 * \brief UpdateBackend implementation for AppService.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "AbstractSettings.h"
#include "AppUpdateData.h"
#include "DownloadService.h"
#include "UpdateService.h"

#include <QJsonObject>
#include <QUrl>

namespace governikus
{

class AppUpdateBackend
	: public QObject
	, public UpdateBackend
{
	Q_OBJECT

	private:
		DownloadService mNotesService;
		AppUpdateData mUpdateData;
		QSharedPointer<QSettings> mStore;

		bool isPlatform(const QString& pPlatform) const;
		bool checkPlatformObject(const QJsonObject& pJson);

	public:
		AppUpdateBackend();

		void skipVersion(const QString& pVersion);
		const AppUpdateData& getUpdateData() const;
		void setTrustedUpdateCertificates(const QVector<QSslCertificate>& pTrustedUpdateCertificates);

		virtual QDateTime getIssueDate() const override;
		virtual void processSuccess(const QByteArray& pData) override;
		virtual void processError(const GlobalStatus& pError) override;

	private Q_SLOTS:
		void onReleaseNotesFinished();

	Q_SIGNALS:
		void fireAppUpdateCheckFinished(bool pUpdateAvailable, const GlobalStatus& pError = GlobalStatus::Code::No_Error);
};


} /* namespace governikus */
