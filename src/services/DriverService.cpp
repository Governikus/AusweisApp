/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "DriverService.h"

#include "AppSettings.h"
#include "DriverParser.h"
#include "DriverSettings.h"
#include "SingletonHelper.h"
#include "UpdateSettingsBackend.h"

using namespace governikus;

defineSingleton(DriverService)

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


DriverService::DriverService()
	: UpdateService(QSharedPointer<UpdateBackend>(new UpdateDriverSettingsBackend(AppSettings::getInstance().getDriverSettings())),
			QStringLiteral("driver configuration"))
{
}


DriverService::~DriverService()
{
}


DriverService& DriverService::getInstance()
{
	return *Instance;
}
