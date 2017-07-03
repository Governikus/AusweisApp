#include "ProviderModel.h"


using namespace governikus;


static const QStringList CATEGORIES({"citizen", "insurance", "finance", "other"});


QString ProviderModel::createCostString(double pCostsPerMinute, double pCostsPerCall)
{
	if (pCostsPerMinute > 0.0)
	{
		return tr("%1/min").arg(createAmountString(pCostsPerMinute));
	}
	if (pCostsPerCall > 0.0)
	{
		return tr("%1/call").arg(createAmountString(pCostsPerCall));
	}
	return QString();
}


QString ProviderModel::createAmountString(double pCents)
{
	return pCents > 100 ? tr("%1 EUR").arg(pCents / 100.0) : tr("%1 ct").arg(pCents);
}


void ProviderModel::onProvidersChanged()
{
	beginResetModel();
	endResetModel();
}


ProviderModel::ProviderModel(ProviderSettings* pSettings, QObject* pParent)
	: QAbstractListModel(pParent)
	, mSettings(pSettings)
{
	connect(mSettings, &ProviderSettings::fireSettingsChanged, this, &ProviderModel::onProvidersChanged);
}


ProviderModel::~ProviderModel()
{
}


int ProviderModel::rowCount(const QModelIndex&) const
{
	return mSettings->getProviders().size();
}


QVariant ProviderModel::data(const QModelIndex& pIndex, int pRole) const
{
	if (pIndex.isValid())
	{
		auto provider = mSettings->getProviders().at(pIndex.row());

		if (pRole == Qt::DisplayRole)
		{
			auto longName = provider.getLongName();
			return longName.isEmpty() ? provider.getShortName().toString() : longName.toString();
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
			const auto& cost = mSettings->getCallCost(provider);
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
			return provider.getIconUrl();
		}
		if (pRole == IMAGE)
		{
			return provider.getImageUrl();
		}
		if (pRole == SORT_ROLE)
		{
			auto value = provider.getLongName();

			return provider.getCategory() + (value.isEmpty() ? provider.getShortName() : value);
		}
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

	return roles;
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
		msg += tr("%1 seconds free, afterwards ").arg(pCosts.getFreeSeconds());
	}
	msg += tr("landline costs %1; ").arg(createCostString(pCosts.getLandlineCentsPerMinute(), pCosts.getLandlineCentsPerCall()));
	const auto mobileCosts = createCostString(pCosts.getMobileCentsPerMinute(), pCosts.getMobileCentsPerCall());
	msg += mobileCosts.isEmpty() ? tr("mobile costs may vary.") : tr("mobile costs %1").arg(mobileCosts);
	return msg;
}
