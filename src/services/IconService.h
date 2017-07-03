/*
 * \brief Class that updates an icon and stores it in the local
 *        application cache.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "ProviderSettings.h"
#include "UpdateService.h"

#include <QObject>


namespace governikus
{
class IconService
	: public UpdateService
{
	Q_OBJECT

	private:
		const QString mIcon;

	public:
		IconService(const QString& pIconCacheBasePath, const QString& pIcon);

		virtual ~IconService();

	private Q_SLOTS:
		void onUpdateFinished();

	Q_SIGNALS:
		void fireProcessingFinished(const QString& pIcon);


};


} /* namespace governikus */
