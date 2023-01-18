/*!
 * \brief Model implementation for the providers.
 *
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ProviderModel.h"

#include <QQmlEngine>
#include <QSet>
#include <QSortFilterProxyModel>
#include <QStringList>

class test_ProviderCategoryFilterModel;

namespace governikus
{

class ProviderCategoryFilterModel
	: public QSortFilterProxyModel
{
	Q_OBJECT
	Q_PROPERTY(QString searchString READ getSearchString WRITE updateSearchString NOTIFY fireCriteriaChanged)
	Q_PROPERTY(QStringList categories READ getSelectedCategories NOTIFY fireCriteriaChanged)
	Q_PROPERTY(int rowCount READ rowCount NOTIFY fireCriteriaChanged)
	Q_PROPERTY(int additionalResultCount READ getAdditionalResultCount NOTIFY fireCriteriaChanged)
	friend class ::test_ProviderCategoryFilterModel;

	private:
		QString mSearchString;
		QStringList mSelectedCategories;

		ProviderModel mProviderModel;

		[[nodiscard]] QString getSearchString() const;
		void updateSearchString(const QString& pSearchString);
		[[nodiscard]] QStringList getSelectedCategories() const;
		[[nodiscard]] int getAdditionalResultCount() const;
		[[nodiscard]] QStringList getFilteredProviderCategories() const;
		[[nodiscard]] int resultCountForFilter(const QStringList& pCategories, const QString& pSearchString) const;
		[[nodiscard]] bool rowMatchesFilter(int pSourceRow, const QModelIndex& pSourceParent, const QStringList& pSelectedCategories, const QString& pSearchString, bool pMatchCategoryType) const;

	protected:
		[[nodiscard]] bool filterAcceptsRow(int pSourceRow, const QModelIndex& pSourceParent) const override;

	public:
		ProviderCategoryFilterModel();
		~ProviderCategoryFilterModel() override = default;

		Q_INVOKABLE void setIncludeCategoriesInModel(bool pIncludeCategories);
		Q_INVOKABLE void sortByCategoryFirst(bool pEnabled);
		Q_INVOKABLE void setCategorySelection(const QString& pCategory);
		Q_INVOKABLE void updateCategorySelection(const QString& pCategory, bool pSelected);
		Q_INVOKABLE void addAdditionalResultCategories();

	Q_SIGNALS:
		void fireCriteriaChanged();
};


} // namespace governikus
