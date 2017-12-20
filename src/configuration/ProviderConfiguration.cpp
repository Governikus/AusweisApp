/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "ProviderConfiguration.h"

#include "Env.h"
#include "FileProvider.h"
#include "ProviderConfigurationParser.h"
#include "SingletonHelper.h"

#include <QFile>
#include <QLoggingCategory>
#include <QRegularExpression>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(configuration)


defineSingleton(ProviderConfiguration)


bool ProviderConfiguration::parseProviderConfiguration()
{
	const QString& path = mUpdatableFile->lookupPath();
	if (!QFile::exists(path))
	{
		qCCritical(configuration) << "ProviderConfiguration file not found";
		return false;
	}

	QFile configFile(path);
	if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qCCritical(configuration) << "Wasn't able to open ProviderConfiguration file";
		return false;
	}
	QByteArray configFileContent = configFile.readAll();

	const auto& callCosts = ProviderConfigurationParser::parseCallCosts(configFileContent);
	if (callCosts.isEmpty())
	{
		qCCritical(configuration) << "Parse error while reading ProviderConfiguration";
		return false;
	}

	const auto& providerConfigurationInfos = ProviderConfigurationParser::parseProvider(configFileContent);
	if (providerConfigurationInfos.isEmpty())
	{
		qCCritical(configuration) << "Parse error while reading ProviderConfiguration";
		return false;
	}

	mCallCosts = callCosts;
	mProviderConfigurationInfos = providerConfigurationInfos;
	return true;
}


void ProviderConfiguration::onFileUpdated()
{
	if (parseProviderConfiguration())
	{
		for (const ProviderConfigurationInfo& info : qAsConst(mProviderConfigurationInfos))
		{
			info.getIcon()->markDirty();
			info.getImage()->markDirty();
		}

		Q_EMIT fireUpdated();
	}
}


ProviderConfiguration::ProviderConfiguration()
	: mUpdatableFile(Env::getSingleton<FileProvider>()->getFile(QString(), QStringLiteral("supported-providers.json")))
	, mProviderConfigurationInfos()
	, mCallCosts()
{
	connect(mUpdatableFile.data(), &UpdatableFile::fireUpdated, this, &ProviderConfiguration::onFileUpdated);
	parseProviderConfiguration();
}


ProviderConfiguration& ProviderConfiguration::getInstance()
{
	return *Instance;
}


void ProviderConfiguration::update()
{
	mUpdatableFile->update();
}


const QVector<ProviderConfigurationInfo>& ProviderConfiguration::getProviderConfigurationInfos() const
{
	return mProviderConfigurationInfos;
}


const CallCost ProviderConfiguration::getCallCost(const ProviderConfigurationInfo& pProvider) const
{
	QString standardisedPhoneNumber = pProvider.getPhone();
	standardisedPhoneNumber = standardisedPhoneNumber.remove(QStringLiteral("+49"));
	standardisedPhoneNumber = standardisedPhoneNumber.remove(QRegularExpression(QStringLiteral("[^\\d]")));

	if (!standardisedPhoneNumber.isEmpty())
	{
		const auto& prefixes = mCallCosts.keys();
		for (const auto& prefix : prefixes)
		{
			if (standardisedPhoneNumber.startsWith(prefix))
			{
				return mCallCosts[prefix];
			}
		}
	}
	return CallCost();
}
