/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "AppUpdateData.h"
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
		void checkAppUpdate();
		const IconCache& getIconCache() const;
		const AppUpdateData& getUpdateData() const;
		void skipVersion(const QString& pVersion);

	private Q_SLOTS:
		void onUpdateProviderSettingsFinished();
		void doUpdateRemoteSettings();

	Q_SIGNALS:
		void fireAppUpdateCheckFinished(bool pUpdateAvailable, const GlobalStatus& pError);
};

} /* namespace governikus */
