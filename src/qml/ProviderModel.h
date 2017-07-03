/*!
 * \brief Model implementation for the providers.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "ProviderSettings.h"

#include <QAbstractListModel>
#include <QPointer>
#include <QSet>
#include <QSharedPointer>
#include <QSortFilterProxyModel>


namespace governikus
{
class ProviderCategoryFilterModel
	: public QSortFilterProxyModel
{
	Q_OBJECT

	private:
		QString mSearchString;

		QSet<QString> mSelectedCategories;

	protected:
		bool filterAcceptsRow(int pSourceRow, const QModelIndex& pSourceParent) const override;

	public:
		ProviderCategoryFilterModel();

		virtual ~ProviderCategoryFilterModel();

		Q_INVOKABLE void updateSearchString(const QString& pSearchString);

		Q_INVOKABLE void updateCategorySelection(const QString& pCategory, bool pSelected);

		Q_INVOKABLE int matchesForExcludedCategory(const QString& pCategory) const;

		Q_INVOKABLE bool isSelected(const QString& pCategory) const;

	Q_SIGNALS:
		void fireCriteriaChanged();

};


class ProviderModel
	: public QAbstractListModel
{
	Q_OBJECT
	Q_PROPERTY(QSortFilterProxyModel * filter READ getFilterModel CONSTANT)
	Q_PROPERTY(QSortFilterProxyModel * sortModel READ getSortModel CONSTANT)
	Q_PROPERTY(ProviderCategoryFilterModel * categoryFilter READ getCategoryFilterModel CONSTANT)

	QPointer<ProviderSettings> mSettings;
	QSortFilterProxyModel mFilterModel;
	QSortFilterProxyModel mSortModel;
	ProviderCategoryFilterModel mCategoryFilterModel;

	private Q_SLOTS:
		void onProvidersChanged();

	public:
		enum ProviderRoles
		{
			CATEGORY = Qt::UserRole + 1,
			SHORTNAME,
			LONGNAME,
			SHORTDESCRIPTION,
			LONGDESCRIPTION,
			ADDRESS,
			ADDRESS_DOMAIN,
			HOMEPAGE,
			HOMEPAGE_BASE,
			PHONE,
			EMAIL,
			POSTALADDRESS,
			ICON,
			IMAGE,
			SORT_ROLE
		};

		ProviderModel(ProviderSettings* pSettings, QObject* pParent = nullptr);
		virtual ~ProviderModel();

		int rowCount(const QModelIndex&) const override;
		QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		QHash<int, QByteArray> roleNames() const override;

		Q_INVOKABLE QSortFilterProxyModel* getFilterModel();
		Q_INVOKABLE QSortFilterProxyModel* getSortModel();
		Q_INVOKABLE ProviderCategoryFilterModel* getCategoryFilterModel();
};


} /* namespace governikus */
