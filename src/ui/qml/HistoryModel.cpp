/*!
 * \brief Model implementation for the history entries.
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "HistoryModel.h"

#include "asn1/AccessRoleAndRight.h"
#include "ProviderConfiguration.h"
#include "ProviderModel.h"

#include <QDebug>
#include <QUrl>

using namespace governikus;


namespace
{
bool matchProviderWithSubjectUrl(const ProviderConfigurationInfo& pProvider, const QString& pSubjectUrl)
{
	const QString subjectUrlHost = QUrl(pSubjectUrl).host();

	// Check provider address host.
	if (QUrl(pProvider.getAddress()).host() == subjectUrlHost)
	{
		return true;
	}

	// Check subject urls.
	for (const auto& subjectUrl : pProvider.getSubjectUrls())
	{
		if (QUrl(subjectUrl).host() == subjectUrlHost)
		{
			return true;
		}
	}

	return false;
}


} // namespace


HistoryProxyModel::HistoryProxyModel()
{
}


HistoryProxyModel::~HistoryProxyModel()
{
}


bool HistoryProxyModel::removeRows(int pRow, int pCount, const QModelIndex& pParent)
{
	return QSortFilterProxyModel::removeRows(pRow, pCount, pParent);
}


bool ProviderNameFilterModel::filterAcceptsRow(int pSourceRow, const QModelIndex& /* pSourceParent */) const
{
	HistoryModel* const dataSourceModel = qobject_cast<HistoryModel*>(sourceModel());
	if (dataSourceModel == nullptr)
	{
		return false;
	}

	auto entry = mHistorySettings->getHistoryInfos()[pSourceRow];

	return matchProviderWithSubjectUrl(mProvider, entry.getSubjectUrl());
}


ProviderNameFilterModel::ProviderNameFilterModel(HistorySettings* pHistorySettings)
	: mHistorySettings(pHistorySettings)
{
}


ProviderNameFilterModel::~ProviderNameFilterModel()
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


HistoryModel::HistoryModel(HistorySettings* pHistorySettings, QObject* pParent)
	: QAbstractListModel(pParent)
	, mHistorySettings(pHistorySettings)
	, mFilterModel()
	, mNameFilterModel(pHistorySettings)
{
	updateConnections();
	mFilterModel.setSourceModel(this);
	mFilterModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
	mNameFilterModel.setSourceModel(this);
	mHistoryModelSearchFilter.setSourceModel(this);
	connect(mHistorySettings.data(), &HistorySettings::fireHistoryInfosChanged, this, &HistoryModel::onHistoryEntriesChanged);
	connect(mHistorySettings.data(), &HistorySettings::fireEnabledChanged, this, &HistoryModel::fireEnabledChanged);
	connect(Env::getSingleton<ProviderConfiguration>(), &ProviderConfiguration::fireUpdated, this, &HistoryModel::onProvidersChanged);
}


HistoryModel::~HistoryModel()
{
}


void HistoryModel::updateConnections()
{
	for (const auto& connection : qAsConst(mConnections))
	{
		disconnect(connection);
	}
	mConnections.clear();

	const auto& historyEntries = mHistorySettings->getHistoryInfos();
	mConnections.reserve(historyEntries.size() * 2);
	for (int i = 0; i < historyEntries.size(); i++)
	{
		const auto& provider = determineProviderFor(historyEntries.at(i));
		const QModelIndex& modelIndex = createIndex(i, 0);

		mConnections += connect(provider.getIcon().data(), &UpdatableFile::fireUpdated, this, [ = ] {
					Q_EMIT dataChanged(modelIndex, modelIndex, {PROVIDER_ICON});
				});
		mConnections += connect(provider.getImage().data(), &UpdatableFile::fireUpdated, this, [ = ] {
					Q_EMIT dataChanged(modelIndex, modelIndex, {PROVIDER_IMAGE});
				});
	}
}


void HistoryModel::onHistoryEntriesChanged()
{
	beginResetModel();
	updateConnections();
	endResetModel();
}


void HistoryModel::onProvidersChanged()
{
	const static QVector<int> PROVIDER_ROLES({PROVIDER_CATEGORY,
											  PROVIDER_SHORTNAME,
											  PROVIDER_LONGNAME,
											  PROVIDER_SHORTDESCRIPTION,
											  PROVIDER_LONGDESCRIPTION,
											  PROVIDER_ADDRESS,
											  PROVIDER_ADDRESS_DOMAIN,
											  PROVIDER_HOMEPAGE,
											  PROVIDER_HOMEPAGE_BASE,
											  PROVIDER_PHONE,
											  PROVIDER_PHONE_COST,
											  PROVIDER_EMAIL,
											  PROVIDER_POSTALADDRESS,
											  PROVIDER_ICON,
											  PROVIDER_IMAGE});
	Q_EMIT dataChanged(index(0), index(rowCount() - 1), PROVIDER_ROLES);
}


int HistoryModel::rowCount(const QModelIndex&) const
{
	return mHistorySettings->getHistoryInfos().size();
}


QVariant HistoryModel::data(const QModelIndex& pIndex, int pRole) const
{
	if (pIndex.isValid() && pIndex.row() < rowCount())
	{
		auto entry = mHistorySettings->getHistoryInfos()[pIndex.row()];
		if (pRole == Qt::DisplayRole || pRole == SUBJECT)
		{
			return entry.getSubjectName();
		}
		if (pRole == PURPOSE)
		{
			return entry.getPurpose();
		}
		if (pRole == DATETIME)
		{
			return entry.getDateTime();
		}
		if (pRole == TERMSOFUSAGE)
		{
			return entry.getTermOfUsage();
		}
		if (pRole == REQUESTEDDATA)
		{
			return AccessRoleAndRightsUtil::joinFromTechnicalName(entry.getRequestedData());
		}
		if (pRole == PROVIDER_CATEGORY)
		{
			auto provider = determineProviderFor(entry);
			return provider.getCategory();
		}
		if (pRole == PROVIDER_SHORTNAME)
		{
			auto provider = determineProviderFor(entry);
			return provider.getShortName().toString();
		}
		if (pRole == PROVIDER_LONGNAME)
		{
			auto provider = determineProviderFor(entry);
			return provider.getLongName().toString();
		}
		if (pRole == PROVIDER_SHORTDESCRIPTION)
		{
			auto provider = determineProviderFor(entry);
			return provider.getShortDescription().toString();
		}
		if (pRole == PROVIDER_LONGDESCRIPTION)
		{
			auto provider = determineProviderFor(entry);
			return provider.getLongDescription().toString();
		}
		if (pRole == PROVIDER_ADDRESS)
		{
			auto provider = determineProviderFor(entry);
			return provider.getAddress();
		}
		if (pRole == PROVIDER_ADDRESS_DOMAIN)
		{
			auto provider = determineProviderFor(entry);
			return provider.getAddressDomain();
		}
		if (pRole == PROVIDER_HOMEPAGE)
		{
			auto provider = determineProviderFor(entry);
			return provider.getHomepage();
		}
		if (pRole == PROVIDER_HOMEPAGE_BASE)
		{
			auto provider = determineProviderFor(entry);
			return provider.getHomepageBase();
		}
		if (pRole == PROVIDER_PHONE)
		{
			auto provider = determineProviderFor(entry);
			return provider.getPhone();
		}
		if (pRole == PROVIDER_PHONE_COST)
		{
			const auto& provider = determineProviderFor(entry);
			const auto& cost = Env::getSingleton<ProviderConfiguration>()->getCallCost(provider);
			return ProviderModel::createCostString(cost);
		}
		if (pRole == PROVIDER_EMAIL)
		{
			auto provider = determineProviderFor(entry);
			return provider.getEMail();
		}
		if (pRole == PROVIDER_POSTALADDRESS)
		{
			auto provider = determineProviderFor(entry);
			return provider.getPostalAddress();
		}
		if (pRole == PROVIDER_ICON)
		{
			auto provider = determineProviderFor(entry);
			return provider.getIcon()->lookupUrl();
		}
		if (pRole == PROVIDER_IMAGE)
		{
			auto provider = determineProviderFor(entry);
			return provider.getImage()->lookupUrl();
		}
	}
	return QVariant();
}


ProviderConfigurationInfo HistoryModel::determineProviderFor(const HistoryInfo& pHistoryInfo) const
{
	QVector<ProviderConfigurationInfo> matchingProviders;
	const auto& providers = Env::getSingleton<ProviderConfiguration>()->getProviderConfigurationInfos();
	for (const auto& provider : providers)
	{
		if (matchProviderWithSubjectUrl(provider, pHistoryInfo.getSubjectUrl()))
		{
			matchingProviders += provider;
		}
	}
	if (matchingProviders.size() == 1)
	{
		return matchingProviders.at(0);
	}
	return ProviderConfigurationInfo();
}


bool HistoryModel::isEnabled() const
{
	return mHistorySettings->isEnabled();
}


void HistoryModel::setEnabled(bool pEnabled)
{
	if (pEnabled != isEnabled())
	{
		mHistorySettings->setEnabled(pEnabled);
		mHistorySettings->save();
	}
}


QHash<int, QByteArray> HistoryModel::roleNames() const
{
	QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
	roles.insert(SUBJECT, "subject");
	roles.insert(PURPOSE, "purpose");
	roles.insert(DATETIME, "dateTime");
	roles.insert(TERMSOFUSAGE, "termsOfUsage");
	roles.insert(REQUESTEDDATA, "requestedData");
	roles.insert(PROVIDER_CATEGORY, "providerCategory");
	roles.insert(PROVIDER_SHORTNAME, "providerShortName");
	roles.insert(PROVIDER_LONGNAME, "providerLongName");
	roles.insert(PROVIDER_SHORTDESCRIPTION, "providerShortDescription");
	roles.insert(PROVIDER_LONGDESCRIPTION, "providerLongDescription");
	roles.insert(PROVIDER_ADDRESS, "providerAddress");
	roles.insert(PROVIDER_ADDRESS_DOMAIN, "providerAddressDomain");
	roles.insert(PROVIDER_HOMEPAGE, "providerHomepage");
	roles.insert(PROVIDER_HOMEPAGE_BASE, "providerHomepageBase");
	roles.insert(PROVIDER_PHONE, "providerPhone");
	roles.insert(PROVIDER_PHONE_COST, "providerPhoneCost");
	roles.insert(PROVIDER_EMAIL, "providerEmail");
	roles.insert(PROVIDER_POSTALADDRESS, "providerPostalAddress");
	roles.insert(PROVIDER_ICON, "providerIcon");
	roles.insert(PROVIDER_IMAGE, "providerImage");
	return roles;
}


bool HistoryModel::removeRows(int pRow, int pCount, const QModelIndex& pParent)
{
	beginRemoveRows(pParent, pRow, pRow + pCount - 1);

	auto entries = mHistorySettings->getHistoryInfos();
	entries.remove(pRow, pCount);

	// disconnect the signal, otherwise this model gets reset
	disconnect(mHistorySettings.data(), &HistorySettings::fireHistoryInfosChanged, this, &HistoryModel::onHistoryEntriesChanged);
	mHistorySettings->setHistoryInfos(entries);
	connect(mHistorySettings.data(), &HistorySettings::fireHistoryInfosChanged, this, &HistoryModel::onHistoryEntriesChanged);

	mHistorySettings->save();

	endRemoveRows();
	return true;
}


HistoryProxyModel* HistoryModel::getFilterModel()
{
	return &mFilterModel;
}


ProviderNameFilterModel* HistoryModel::getNameFilterModel()
{
	return &mNameFilterModel;
}


HistoryModelSearchFilter* HistoryModel::getHistoryModelSearchFilter()
{
	return &mHistoryModelSearchFilter;
}
