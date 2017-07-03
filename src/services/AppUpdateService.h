/*!
 * \brief Update checker for the application.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "AppUpdateData.h"
#include "UpdateService.h"

namespace governikus
{

class AppUpdateService
	: public UpdateService
{
	Q_OBJECT

	protected:
		AppUpdateService();
		virtual ~AppUpdateService();

	public:
		static AppUpdateService& getInstance();

		void skipVersion(const QString& pVersion);
		const AppUpdateData& getUpdateData() const;
		virtual void setTrustedUpdateCertificates(const QVector<QSslCertificate>& pTrustedUpdateCertificates) override;

	Q_SIGNALS:
		void fireAppUpdateCheckFinished(bool pUpdateAvailable, const GlobalStatus& pError);
};


} /* namespace governikus */
