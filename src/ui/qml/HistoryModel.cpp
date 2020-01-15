/*!
 * \brief Model implementation for the history entries.
 *
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#include "HistoryModel.h"

#include "asn1/AccessRoleAndRight.h"
#include "AppSettings.h"
#include "Env.h"

#include "PdfExporter.h"
#include "ProviderConfiguration.h"
#include "ProviderModel.h"

#include <QQmlEngine>

using namespace governikus;


auto& HistoryModel::getHistorySettings()
{
	return Env::getSingleton<AppSettings>()->getHistorySettings();
}


HistoryModel::HistoryModel(QObject* pParent)
	: QAbstractListModel(pParent)
	, mFilterModel()
	, mNameFilterModel()
	, mHistoryModelSearchFilter()
{
	updateConnections();
	mFilterModel.setSourceModel(this);
	mFilterModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
	mNameFilterModel.setSourceModel(this);
	mHistoryModelSearchFilter.setSourceModel(this);

	const auto& historySettings = getHistorySettings();
	connect(&historySettings, &HistorySettings::fireHistoryInfosChanged, this, &HistoryModel::onHistoryEntriesChanged);
	connect(&historySettings, &HistorySettings::fireEnabledChanged, this, &HistoryModel::fireEnabledChanged);
	connect(Env::getSingleton<ProviderConfiguration>(), &ProviderConfiguration::fireUpdated, this, &HistoryModel::onProvidersChanged);

	QQmlEngine::setObjectOwnership(&mFilterModel, QQmlEngine::CppOwnership);
	QQmlEngine::setObjectOwnership(&mNameFilterModel, QQmlEngine::CppOwnership);
	QQmlEngine::setObjectOwnership(&mHistoryModelSearchFilter, QQmlEngine::CppOwnership);
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

	const auto& historyEntries = getHistorySettings().getHistoryInfos();
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
	Q_EMIT fireEmptyChanged(isEmpty());
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
	return getHistorySettings().getHistoryInfos().size();
}


QVariant HistoryModel::data(const QModelIndex& pIndex, int pRole) const
{
	if (pIndex.isValid() && pIndex.row() < rowCount())
	{
		const auto& infos = getHistorySettings().getHistoryInfos();
		const auto& entry = infos[pIndex.row()];
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
			auto tos = entry.getTermOfUsage();
			return tos.remove(QLatin1Char('\r')).replace(QLatin1Char('\t'), QLatin1Char(' '));
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
		if (provider.matchWithSubjectUrl(pHistoryInfo.getSubjectUrl()))
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
	return getHistorySettings().isEnabled();
}


void HistoryModel::setEnabled(bool pEnabled)
{
	if (pEnabled != isEnabled())
	{
		auto& historySettings = getHistorySettings();
		historySettings.setEnabled(pEnabled);
		historySettings.save();
	}
}


bool HistoryModel::isEmpty() const
{
	const auto& historyEntries = getHistorySettings().getHistoryInfos();
	return historyEntries.isEmpty();
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
	if (pCount <= 0 || pRow < 0)
	{
		return false;
	}
	beginRemoveRows(pParent, pRow, pRow + pCount - 1);

	auto& historySettings = getHistorySettings();
	auto entries = historySettings.getHistoryInfos();
	entries.remove(pRow, pCount);

	// disconnect the signal, otherwise this model gets reset
	disconnect(&historySettings, &HistorySettings::fireHistoryInfosChanged, this, &HistoryModel::onHistoryEntriesChanged);
	historySettings.setHistoryInfos(entries);
	connect(&historySettings, &HistorySettings::fireHistoryInfosChanged, this, &HistoryModel::onHistoryEntriesChanged);

	historySettings.save();

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


void HistoryModel::exportHistory(const QUrl& pFilename) const
{
	PdfExporter exporter(pFilename.toLocalFile());
	exporter.exportHistory();
}


#ifndef QT_NO_DEBUG
void HistoryModel::createDummyEntry()
{
	getHistorySettings().addHistoryInfo(HistoryInfo(
			QStringLiteral("Dummy Subject"), QStringLiteral("Dummy Subject URL"), QStringLiteral("Dummy Usage"),
			QDateTime::currentDateTime(), QStringLiteral("Dummy Term Of Usage"),
			{QStringLiteral("Dummy Data 1"), QStringLiteral("Dummy Data 2")}));
}


#endif
