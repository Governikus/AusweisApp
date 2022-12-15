/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "ProviderConfiguration.h"

#include "FileProvider.h"
#include "ProviderConfigurationParser.h"

#include <QFile>
#include <QLoggingCategory>
#include <QRegularExpression>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(configuration)


bool ProviderConfiguration::parseProviderConfiguration(const QString& pPath)
{
	if (!QFile::exists(pPath))
	{
		qCCritical(configuration) << "ProviderConfiguration file not found:" << pPath;
		return false;
	}

	QFile configFile(pPath);
	if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qCCritical(configuration) << "Wasn't able to open ProviderConfiguration file:" << pPath;
		return false;
	}
	QByteArray configFileContent = configFile.readAll();

	const auto& callCosts = ProviderConfigurationParser::parseCallCosts(configFileContent);
	if (callCosts.isEmpty())
	{
		qCCritical(configuration) << "Parse error while reading ProviderConfiguration:" << pPath;
		return false;
	}

	const auto& providerConfigurationInfos = ProviderConfigurationParser::parseProvider(configFileContent);
	if (providerConfigurationInfos.isEmpty())
	{
		qCCritical(configuration) << "Parse error while reading ProviderConfiguration:" << pPath;
		return false;
	}

	mCallCosts = callCosts;
	mProviderConfigurationInfos = providerConfigurationInfos;
	return true;
}


void ProviderConfiguration::onFileUpdated()
{
	if (mUpdatableFile->forEachLookupPath([this](const QString& pPath){return parseProviderConfiguration(pPath);}))
	{
		for (const ProviderConfigurationInfo& info : std::as_const(mProviderConfigurationInfos))
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
	connect(mUpdatableFile.data(), &UpdatableFile::fireNoUpdateAvailable, this, &ProviderConfiguration::fireNoUpdateAvailable);
	mUpdatableFile->forEachLookupPath([this](const QString& pPath){return parseProviderConfiguration(pPath);});
}


void ProviderConfiguration::update()
{
	mUpdatableFile->update();
}


const QVector<ProviderConfigurationInfo>& ProviderConfiguration::getProviderConfigurationInfos() const
{
	return mProviderConfigurationInfos;
}


CallCost ProviderConfiguration::getCallCost(const ProviderConfigurationInfo& pProvider) const
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


ProviderConfigurationInfo ProviderConfiguration::getProviderInfo(const QString& pInternalId) const
{
	for (const auto& info : std::as_const(mProviderConfigurationInfos))
	{
		if (info.getInternalId() == pInternalId)
		{
			return info;
		}
	}

	return ProviderConfigurationInfo();
}
