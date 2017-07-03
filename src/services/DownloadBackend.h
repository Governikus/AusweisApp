/*!
 * \brief DownloadBackend implementation for \ref DownloadService.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "UpdateBackend.h"

namespace governikus
{

class DownloadBackend
	: public UpdateBackend
{
	private:
		QByteArray mContent;
		GlobalStatus mError;

	public:
		DownloadBackend()
			: mError(GlobalStatus::Code::No_Error)
		{

		}


		const QByteArray& getContent() const;
		const GlobalStatus& getError() const;

		virtual QDateTime getIssueDate() const override;
		virtual void processSuccess(const QByteArray& pData) override;
		virtual void processError(const GlobalStatus& pError) override;
};

} /* namespace governikus */
