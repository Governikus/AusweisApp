/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QSet>
#include <QSortFilterProxyModel>
#include <QString>


namespace governikus
{

class LogFilterModel
	: public QSortFilterProxyModel
{
	Q_OBJECT
	Q_PROPERTY(QSet<QString> levels READ getLevels NOTIFY fireLevelsChanged)
	Q_PROPERTY(QSet<QString> selectedLevels READ getSelectedLevels NOTIFY fireLevelsChanged)
	Q_PROPERTY(QSet<QString> categories READ getCategories NOTIFY fireCategoriesChanged)
	Q_PROPERTY(QSet<QString> selectedCategories READ getSelectedCategories NOTIFY fireCategoriesChanged)

	private:
		QSet<QString> mSelectedLevels;
		QSet<QString> mSelectedCategories;

	private Q_SLOTS:
		void onLevelsChanged();
		void onCategoriesChanged();

	protected:
		[[nodiscard]] bool filterAcceptsRow(int pSourceRow, const QModelIndex& pSourceParent) const override;

	public:
		LogFilterModel();
		~LogFilterModel() override = default;

		[[nodiscard]] const QSet<QString>& getLevels() const;
		[[nodiscard]] const QSet<QString>& getSelectedLevels() const;
		[[nodiscard]] const QSet<QString>& getCategories() const;
		[[nodiscard]] const QSet<QString>& getSelectedCategories() const;

		Q_INVOKABLE void configureLevel(const QString& pLevel, bool pEnabled);
		Q_INVOKABLE void configureCategory(const QString& pCategory, bool pEnabled);

	Q_SIGNALS:
		void fireLevelsChanged();
		void fireCategoriesChanged();
};

} // namespace governikus
