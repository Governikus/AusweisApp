/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "IconCache.h"

namespace governikus
{
class Updater
	: public QObject
{
	Q_OBJECT

	private:
		IconCache mIconCache;

	protected:
		Updater();

	public:
		static Updater& getInstance();

		void update();
		void checkAppUpdate(bool pSilent = false);
		const IconCache& getIconCache() const;

	private Q_SLOTS:
		void onUpdateProviderSettingsFinished();
		void doUpdateRemoteSettings();
};

} /* namespace governikus */
