#include "ProviderModel.h"

using namespace governikus;


bool ProviderCategoryFilterModel::filterAcceptsRow(int pSourceRow, const QModelIndex& pSourceParent) const
{
	QAbstractItemModel* const model = sourceModel();
	Q_ASSERT(model != nullptr);
	const QModelIndex idx = model->index(pSourceRow, 0, pSourceParent);

	if (!mSearchString.isEmpty())
	{
		const QString dt = model->data(idx, Qt::DisplayRole).toString().toLower();
		if (!dt.contains(mSearchString))
		{
			return false;
		}
	}

	return mSelectedCategories.isEmpty() ||
		   mSelectedCategories.contains(model->data(idx, ProviderModel::CATEGORY).toString().toLower());
}


ProviderCategoryFilterModel::ProviderCategoryFilterModel()
{
}


ProviderCategoryFilterModel::~ProviderCategoryFilterModel()
{
}


void ProviderCategoryFilterModel::updateSearchString(const QString& pSearchString)
{
	const QString lowerSearchString = pSearchString.toLower();
	if (mSearchString != lowerSearchString)
	{
		mSearchString = lowerSearchString;

		invalidateFilter();
		Q_EMIT fireCriteriaChanged();
	}
}


void ProviderCategoryFilterModel::updateCategorySelection(const QString& pCategory, bool pSelected)
{
	const int categoryCount = mSelectedCategories.count();
	if (pSelected)
	{
		mSelectedCategories.insert(pCategory.toLower());
	}
	else
	{
		mSelectedCategories.remove(pCategory.toLower());
	}

	if (mSelectedCategories.count() != categoryCount)
	{
		invalidateFilter();

		fireCriteriaChanged();
	}
}


int ProviderCategoryFilterModel::matchesForExcludedCategory(const QString& pCategory) const
{
	if (mSearchString.isEmpty() || mSelectedCategories.isEmpty() || mSelectedCategories.contains(pCategory))
	{
		return 0;
	}

	QAbstractItemModel* const model = sourceModel();
	Q_ASSERT(model != nullptr);
	const int count = model->rowCount();
	int matchCount = 0;
	for (int sourceRow = 0; sourceRow < count; ++sourceRow)
	{
		const QModelIndex idx = model->index(sourceRow, 0, QModelIndex());
		if (!mSearchString.isEmpty())
		{
			const QString dt = model->data(idx, Qt::DisplayRole).toString().toLower();
			if (!dt.contains(mSearchString))
			{
				continue;
			}
		}

		if (pCategory.toLower() == model->data(idx, ProviderModel::CATEGORY).toString().toLower())
		{
			matchCount++;
		}
	}

	return matchCount;
}


bool ProviderCategoryFilterModel::isSelected(const QString& pCategory) const
{
	return mSelectedCategories.contains(pCategory);
}


ProviderModel::ProviderModel(ProviderSettings* pSettings, QObject* pParent)
	: QAbstractListModel(pParent)
	, mSettings(pSettings)
	, mFilterModel()
	, mSortModel()
	, mCategoryFilterModel()
{
	mFilterModel.setSourceModel(this);
	mFilterModel.sort(0);
	mFilterModel.setFilterRole(CATEGORY);

	mSortModel.setSourceModel(this);
	mSortModel.sort(0);
	mSortModel.setSortRole(SORT_ROLE);

	mCategoryFilterModel.setSourceModel(this);

	connect(mSettings, &ProviderSettings::fireProvidersChanged, this, &ProviderModel::onProvidersChanged);
}


ProviderModel::~ProviderModel()
{
}


void ProviderModel::onProvidersChanged()
{
	beginResetModel();
	endResetModel();
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
			auto value = provider.getLongName();
			return QVariant(value.isEmpty() ? provider.getShortName() : value);
		}

		if (pRole == CATEGORY)
		{
			return provider.getCategory();
		}
		if (pRole == SHORTNAME)
		{
			return QVariant(provider.getShortName());
		}
		if (pRole == LONGNAME)
		{
			return QVariant(provider.getLongName());
		}
		if (pRole == SHORTDESCRIPTION)
		{
			return QVariant(provider.getShortDescription());
		}
		if (pRole == LONGDESCRIPTION)
		{
			return QVariant(provider.getLongDescription());
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
	roles.insert(CATEGORY, "category");
	roles.insert(SHORTNAME, "shortName");
	roles.insert(LONGNAME, "longName");
	roles.insert(SHORTDESCRIPTION, "shortDescription");
	roles.insert(LONGDESCRIPTION, "longDescription");
	roles.insert(ADDRESS, "address");
	roles.insert(ADDRESS_DOMAIN, "addressDomain");
	roles.insert(HOMEPAGE, "homepage");
	roles.insert(HOMEPAGE_BASE, "homepagebase");
	roles.insert(PHONE, "phone");
	roles.insert(EMAIL, "email");
	roles.insert(POSTALADDRESS, "postalAddress");
	roles.insert(ICON, "icon");
	roles.insert(IMAGE, "image");

	return roles;
}


QSortFilterProxyModel* ProviderModel::getFilterModel()
{
	return &mFilterModel;
}


QSortFilterProxyModel* ProviderModel::getSortModel()
{
	return &mSortModel;
}


ProviderCategoryFilterModel* ProviderModel::getCategoryFilterModel()
{
	return &mCategoryFilterModel;
}
