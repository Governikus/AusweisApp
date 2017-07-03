/*!
 * DriverService.cpp
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */


#include "DriverService.h"

#include "DriverParser.h"
#include "UpdateSettingsBackend.h"

#include "AppSettings.h"

#include <QMutexLocker>

using namespace governikus;

namespace
{
class UpdateDriverSettingsBackend
	: public UpdateSettingsBackend<DriverSettings>
{
	public:
		UpdateDriverSettingsBackend(DriverSettings& pSettings)
			: UpdateSettingsBackend<DriverSettings>(pSettings,
					QSharedPointer<SettingsParser<DriverSettings> >(new DriverParser()),
					QStringLiteral("default-supported-devices.json"),
					QStringLiteral("driver configuration"))
		{
		}


		virtual ~UpdateDriverSettingsBackend()
		{
		}


};

}


DriverService* DriverService::mSharedInstance = nullptr;


DriverService::DriverService(DriverSettings& pSettings)
	: UpdateService(QSharedPointer<UpdateBackend>(new UpdateDriverSettingsBackend(pSettings)),
			QStringLiteral("driver configuration"))
{
}


DriverService::~DriverService()
{
}


DriverService* DriverService::getSharedInstance()
{
	static QMutex mutex;
	if (!mSharedInstance)
	{
		QMutexLocker locker(&mutex);
		if (!mSharedInstance)
		{
			mSharedInstance = new DriverService(AppSettings::getInstance().getDriverSettings());
		}
	}

	return mSharedInstance;
}
