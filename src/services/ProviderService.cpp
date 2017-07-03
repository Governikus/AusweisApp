/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "ProviderService.h"

#include "AppSettings.h"
#include "ProviderParser.h"
#include "ProviderSettings.h"
#include "SingletonHelper.h"
#include "UpdateSettingsBackend.h"

using namespace governikus;

defineSingleton(ProviderService)

namespace
{
class UpdateProviderSettingsBackend
	: public UpdateSettingsBackend<ProviderSettings>
{
	public:
		UpdateProviderSettingsBackend(ProviderSettings& pSettings)
			: UpdateSettingsBackend<ProviderSettings>(pSettings,
					QSharedPointer<SettingsParser<ProviderSettings> >(new ProviderParser()),
					QStringLiteral("default-providers.json"),
					QStringLiteral("provider configuration"))
		{
		}


		virtual ~UpdateProviderSettingsBackend()
		{
		}


};

}

ProviderService::ProviderService()
	: UpdateService(QSharedPointer<UpdateBackend>(new UpdateProviderSettingsBackend(AppSettings::getInstance().getProviderSettings())),
			QStringLiteral("provider configuration"))
{
}


ProviderService::~ProviderService()
{
}


ProviderService& ProviderService::getInstance()
{
	return *Instance;
}
