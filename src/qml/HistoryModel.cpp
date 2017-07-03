/*!
 * \brief Model implementation for the history entries.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "HistoryModel.h"

#include <QUrl>

using namespace governikus;


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
	HistoryModel* const dataSourceModel = dynamic_cast<HistoryModel*>(sourceModel());
	if (dataSourceModel == nullptr)
	{
		return false;
	}

	auto entry = mHistorySettings->getHistoryEntries()[pSourceRow];

	return mProviderAddressHost == QUrl(entry.getSubjectUrl()).host();
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
	const QString providerAddressHost = QUrl(pProviderAddress).host();
	if (providerAddressHost != mProviderAddressHost)
	{
		mProviderAddressHost = providerAddressHost;

		invalidateFilter();
	}
}


HistoryModel::HistoryModel(HistorySettings* pHistorySettings, ProviderSettings* pProviderSettings, QObject* pParent)
	: QAbstractListModel(pParent)
	, mHistorySettings(pHistorySettings)
	, mProviderSettings(pProviderSettings)
	, mFilterModel()
	, mNameFilterModel(pHistorySettings)
{
	mFilterModel.setSourceModel(this);
	mFilterModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
	mNameFilterModel.setSourceModel(this);
	connect(mHistorySettings, &HistorySettings::fireHistoryEntriesChanged, this, &HistoryModel::onHistoryEntriesChanged);
	connect(mProviderSettings, &ProviderSettings::fireProvidersChanged, this, &HistoryModel::onProvidersChanged);
}


HistoryModel::~HistoryModel()
{
}


void HistoryModel::onHistoryEntriesChanged()
{
	beginResetModel();
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
											  PROVIDER_EMAIL,
											  PROVIDER_POSTALADDRESS,
											  PROVIDER_ICON,
											  PROVIDER_IMAGE});
	dataChanged(index(0), index(rowCount() - 1), PROVIDER_ROLES);
}


int HistoryModel::rowCount(const QModelIndex&) const
{
	return mHistorySettings->getHistoryEntries().size();
}


QVariant HistoryModel::data(const QModelIndex& pIndex, int pRole) const
{
	if (pIndex.isValid() && pIndex.row() < rowCount())
	{
		auto entry = mHistorySettings->getHistoryEntries()[pIndex.row()];
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
			return entry.getRequestedData();
		}
		if (pRole == PROVIDER_CATEGORY)
		{
			auto provider = determineProviderFor(entry);
			return provider.getCategory();
		}
		if (pRole == PROVIDER_SHORTNAME)
		{
			auto provider = determineProviderFor(entry);
			return QVariant(provider.getShortName());
		}
		if (pRole == PROVIDER_LONGNAME)
		{
			auto provider = determineProviderFor(entry);
			return QVariant(provider.getLongName());
		}
		if (pRole == PROVIDER_SHORTDESCRIPTION)
		{
			auto provider = determineProviderFor(entry);
			return QVariant(provider.getShortDescription());
		}
		if (pRole == PROVIDER_LONGDESCRIPTION)
		{
			auto provider = determineProviderFor(entry);
			return QVariant(provider.getLongDescription());
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
			return provider.getIconUrl();
		}
		if (pRole == PROVIDER_IMAGE)
		{
			auto provider = determineProviderFor(entry);
			return provider.getImageUrl();
		}
	}
	return QVariant();
}


Provider HistoryModel::determineProviderFor(const HistoryEntry& pHistoryEntry) const
{
	QVector<Provider> matchingProviders;
	for (const auto& provider : mProviderSettings->getProviders())
	{
		if (QUrl(provider.getAddress()).host() == QUrl(pHistoryEntry.getSubjectUrl()).host())
		{
			matchingProviders.append(provider);
		}
	}
	if (matchingProviders.size() == 1)
	{
		return matchingProviders.at(0);
	}
	return Provider();
}


QHash<int, QByteArray> HistoryModel::roleNames() const
{
	QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
	roles.insert(SUBJECT, "subject");
	roles.insert(PURPOSE, "purpose");
	roles.insert(DATETIME, "dateTime");
	roles.insert(TERMSOFUSAGE, "termsOfUsage");
	roles.insert(REQUESTEDDATA, "requestedData");
	roles.insert(PROVIDER_CATEGORY, "provider_category");
	roles.insert(PROVIDER_SHORTNAME, "provider_shortname");
	roles.insert(PROVIDER_LONGNAME, "provider_longname");
	roles.insert(PROVIDER_SHORTDESCRIPTION, "provider_shortdescription");
	roles.insert(PROVIDER_LONGDESCRIPTION, "provider_longdescription");
	roles.insert(PROVIDER_ADDRESS, "provider_address");
	roles.insert(PROVIDER_ADDRESS_DOMAIN, "provider_address_domain");
	roles.insert(PROVIDER_HOMEPAGE, "provider_homepage");
	roles.insert(PROVIDER_HOMEPAGE_BASE, "provider_homepage_base");
	roles.insert(PROVIDER_PHONE, "provider_phone");
	roles.insert(PROVIDER_EMAIL, "provider_email");
	roles.insert(PROVIDER_POSTALADDRESS, "provider_postaladdress");
	roles.insert(PROVIDER_ICON, "provider_icon");
	roles.insert(PROVIDER_IMAGE, "provider_image");
	return roles;
}


bool HistoryModel::removeRows(int pRow, int pCount, const QModelIndex& pParent)
{
	beginRemoveRows(pParent, pRow, pRow + pCount - 1);

	auto entries = mHistorySettings->getHistoryEntries();
	entries.remove(pRow, pCount);

	// disconnect the signal, otherwise this model gets reset
	disconnect(mHistorySettings, &HistorySettings::fireHistoryEntriesChanged, this, &HistoryModel::onHistoryEntriesChanged);
	mHistorySettings->setHistoryEntries(entries);
	connect(mHistorySettings, &HistorySettings::fireHistoryEntriesChanged, this, &HistoryModel::onHistoryEntriesChanged);

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
