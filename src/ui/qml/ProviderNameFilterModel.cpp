/*!
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#include "ProviderNameFilterModel.h"

#include "AppSettings.h"
#include "Env.h"
#include "HistoryModel.h"
#include "HistorySettings.h"
#include "ProviderConfiguration.h"
#include "ProviderModel.h"

#include <QUrl>

using namespace governikus;


bool ProviderNameFilterModel::filterAcceptsRow(int pSourceRow, const QModelIndex& /* pSourceParent */) const
{
	const auto* const dataSourceModel = qobject_cast<HistoryModel*>(sourceModel());
	if (dataSourceModel == nullptr)
	{
		return false;
	}

	const auto& infos = Env::getSingleton<AppSettings>()->getHistorySettings().getHistoryInfos();
	if (pSourceRow >= infos.size())
	{
		return false;
	}

	const auto& entry = infos.at(pSourceRow);
	return mProvider.matchWithSubjectUrl(entry.getSubjectUrl());
}


ProviderNameFilterModel::ProviderNameFilterModel()
	: mProvider()
{
}


void ProviderNameFilterModel::setProviderAddress(const QString& pProviderAddress)
{
	if (mProvider.getAddress() != pProviderAddress)
	{
		const auto& providers = Env::getSingleton<ProviderConfiguration>()->getProviderConfigurationInfos();
		for (const auto& provider : providers)
		{
			if (provider.getAddress() == pProviderAddress)
			{
				mProvider = provider;

				invalidateFilter();

				return;
			}
		}

		// If we get here, no provider for pProviderAddress was found in the settings.
		qWarning() << "Cannot select provider with address" << pProviderAddress;
	}
}
