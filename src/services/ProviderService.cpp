/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "ProviderService.h"

#include "AppSettings.h"
#include "ProviderParser.h"
#include "UpdateSettingsBackend.h"

#include <QMutexLocker>

using namespace governikus;

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


ProviderService* ProviderService::mSharedInstance = nullptr;

ProviderService::ProviderService(ProviderSettings& pSettings)
	: UpdateService(QSharedPointer<UpdateBackend>(new UpdateProviderSettingsBackend(pSettings)),
			QStringLiteral("provider configuration"))
{
}


ProviderService::~ProviderService()
{
}


ProviderService* ProviderService::getSharedInstance()
{
	static QMutex mutex;
	if (!mSharedInstance)
	{
		QMutexLocker locker(&mutex);
		if (!mSharedInstance)
		{
			mSharedInstance = new ProviderService(AppSettings::getInstance().getProviderSettings());
		}
	}
	return mSharedInstance;
}
