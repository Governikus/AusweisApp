/*!
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#include "ProviderModel.h"

#include "ProviderConfiguration.h"


using namespace governikus;


QString ProviderModel::createCostString(double pCostsPerMinute, double pCostsPerCall)
{
	if (pCostsPerMinute > 0.0)
	{
		//: INFO ALL_PLATFORMS Unit for expenses for calling the hotline (per minute).
		return tr("%1/min").arg(createAmountString(pCostsPerMinute));
	}
	if (pCostsPerCall > 0.0)
	{
		//: INFO ALL_PLATFORMS Unit for expenses for calling the hotline (per call).
		return tr("%1/call").arg(createAmountString(pCostsPerCall));
	}
	return QString();
}


QString ProviderModel::createAmountString(double pCents)
{
	//: INFO ALL_PLATFORMS Currency unit for expenses for calling the hotline (Euro/Cent).
	return pCents > 100 ? tr("%1 EUR").arg(pCents / 100.0) : tr("%1 ct").arg(pCents);
}


void ProviderModel::updateConnections()
{
	for (const auto& connection : qAsConst(mConnections))
	{
		disconnect(connection);
	}
	mConnections.clear();

	const auto& providerConfigurationInfos = Env::getSingleton<ProviderConfiguration>()->getProviderConfigurationInfos();
	mConnections.reserve(providerConfigurationInfos.size() * 2);
	for (int i = 0; i < providerConfigurationInfos.size(); i++)
	{
		const auto& provider = providerConfigurationInfos.at(i);
		const QModelIndex& modelIndex = createIndex(i, 0);

		mConnections += connect(provider.getIcon().data(), &UpdatableFile::fireUpdated, this, [this, modelIndex] {
				Q_EMIT dataChanged(modelIndex, modelIndex, {ProviderRoles::ICON});
			});
		mConnections += connect(provider.getImage().data(), &UpdatableFile::fireUpdated, this, [this, modelIndex] {
				Q_EMIT dataChanged(modelIndex, modelIndex, {ProviderRoles::IMAGE});
			});
	}
}


void ProviderModel::onProvidersChanged()
{
	beginResetModel();
	updateConnections();
	endResetModel();
}


ProviderModel::ProviderModel(QObject* pParent)
	: QAbstractListModel(pParent)
	, mIncludeCategories(false)
{
	updateConnections();
	connect(Env::getSingleton<ProviderConfiguration>(), &ProviderConfiguration::fireUpdated, this, &ProviderModel::onProvidersChanged);
}


ProviderModel::~ProviderModel()
{
}


int ProviderModel::rowCount(const QModelIndex&) const
{
	return Env::getSingleton<ProviderConfiguration>()->getProviderConfigurationInfos().size() + (mIncludeCategories ? getProviderCategories().size() : 0);
}


QVariant ProviderModel::data(const QModelIndex& pIndex, int pRole) const
{
	if (!pIndex.isValid())
	{
		return {};
	}

	const auto& providerConfiguration = Env::getSingleton<ProviderConfiguration>();
	const auto& providerConfigurationInfos = providerConfiguration->getProviderConfigurationInfos();

	if (pIndex.row() >= providerConfigurationInfos.size())
	{
		const auto categoryRow = pIndex.row() - providerConfigurationInfos.size();
		const auto category = getProviderCategories().at(categoryRow);

		switch (pRole)
		{
			case TYPE:
				return QStringLiteral("category");

			case CATEGORY:
			case SORT_ROLE:
				return category;

			default:
				return QVariant();
		}
	}

	const auto& provider = providerConfigurationInfos.at(pIndex.row());

	if (pRole == TYPE)
	{
		return QStringLiteral("provider");
	}
	if (pRole == Qt::DisplayRole)
	{
		return provider.getLongName().toString();
	}
	if (pRole == CATEGORY)
	{
		return provider.getCategory();
	}
	if (pRole == SHORTNAME)
	{
		return provider.getShortName().toString();
	}
	if (pRole == LONGNAME)
	{
		return provider.getLongName().toString();
	}
	if (pRole == SHORTDESCRIPTION)
	{
		return provider.getShortDescription().toString();
	}
	if (pRole == LONGDESCRIPTION)
	{
		return provider.getLongDescription().toString();
	}
	if (pRole == ADDRESS)
	{
		return provider.getAddress();
	}
	if (pRole == ADDRESS_DOMAIN)
	{
		return provider.getAddressDomain();
	}
	if (pRole == HOMEPAGE)
	{
		return provider.getHomepage();
	}
	if (pRole == HOMEPAGE_BASE)
	{
		return provider.getHomepageBase();
	}
	if (pRole == PHONE)
	{
		return provider.getPhone();
	}
	if (pRole == PHONE_COST)
	{
		const auto& cost = providerConfiguration->getCallCost(provider);
		return createCostString(cost);
	}
	if (pRole == EMAIL)
	{
		return provider.getEMail();
	}
	if (pRole == POSTALADDRESS)
	{
		return provider.getPostalAddress();
	}
	if (pRole == ICON)
	{
		return provider.getIcon()->lookupUrl();
	}
	if (pRole == IMAGE)
	{
		return provider.getImage()->lookupUrl();
	}
	if (pRole == SORT_ROLE)
	{
		const auto& value = provider.getLongName();
		return provider.getCategory() + (value.isEmpty() ? provider.getShortName() : value);
	}

	return QVariant();
}


QHash<int, QByteArray> ProviderModel::roleNames() const
{
	QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
	roles.insert(CATEGORY, "providerCategory");
	roles.insert(SHORTNAME, "providerShortName");
	roles.insert(LONGNAME, "providerLongName");
	roles.insert(SHORTDESCRIPTION, "providerShortDescription");
	roles.insert(LONGDESCRIPTION, "providerLongDescription");
	roles.insert(ADDRESS, "providerAddress");
	roles.insert(ADDRESS_DOMAIN, "providerAddressDomain");
	roles.insert(HOMEPAGE, "providerHomepage");
	roles.insert(HOMEPAGE_BASE, "providerHomepageBase");
	roles.insert(PHONE, "providerPhone");
	roles.insert(PHONE_COST, "providerPhoneCost");
	roles.insert(EMAIL, "providerEmail");
	roles.insert(POSTALADDRESS, "providerPostalAddress");
	roles.insert(ICON, "providerIcon");
	roles.insert(IMAGE, "providerImage");
	roles.insert(TYPE, "itemType");

	return roles;
}


void ProviderModel::setIncludeCategories(bool pIncludeCategories)
{
	beginResetModel();
	mIncludeCategories = pIncludeCategories;
	endResetModel();
}


bool ProviderModel::getIncludeCategories() const
{
	return mIncludeCategories;
}


const QStringList& ProviderModel::getProviderCategories()
{
	static QStringList cats({QStringLiteral("all"), QStringLiteral("citizen"), QStringLiteral("insurance"), QStringLiteral("finance"), QStringLiteral("other")});
	return cats;
}


QString ProviderModel::createCostString(const CallCost& pCosts)
{
	if (pCosts.isNull())
	{
		return QString();
	}

	QString msg;
	if (pCosts.getFreeSeconds() > 0)
	{
		//: INFO ALL_PLATFORMS Free of charge seconds when calling the hotline.
		msg += tr("%1 seconds free, afterwards ").arg(pCosts.getFreeSeconds());
	}
	//: INFO ALL_PLATFORMS Land line charges when calling the hotline.
	msg += tr("landline costs %1; ").arg(createCostString(pCosts.getLandlineCentsPerMinute(), pCosts.getLandlineCentsPerCall()));
	const auto mobileCosts = createCostString(pCosts.getMobileCentsPerMinute(), pCosts.getMobileCentsPerCall());
	//: INFO ALL_PLATFORMS Cell phone charges when calling the hotline.
	msg += mobileCosts.isEmpty() ? tr("mobile costs may vary.") : tr("mobile costs %1").arg(mobileCosts);
	return msg;
}
