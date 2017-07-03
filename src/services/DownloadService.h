/*!
 * \brief General downloader as service.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "UpdateService.h"

namespace governikus
{

class DownloadService
	: public UpdateService
{
	Q_OBJECT

	public:
		DownloadService();
		virtual ~DownloadService();

		const QByteArray& getContent() const;
		const GlobalStatus& getError() const;
};


} /* namespace governikus */
