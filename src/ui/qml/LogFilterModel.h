/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QSet>
#include <QSortFilterProxyModel>
#include <QStringList>
#include <QtQml/qqmlregistration.h>


namespace governikus
{

class LogFilterModel
	: public QSortFilterProxyModel
{
	Q_OBJECT
	QML_ELEMENT

	Q_PROPERTY(QStringList levels READ getLevels NOTIFY fireLevelsChanged)
	Q_PROPERTY(QStringList selectedLevels READ getSelectedLevels NOTIFY fireLevelsChanged)
	Q_PROPERTY(QStringList categories READ getCategories NOTIFY fireCategoriesChanged)
	Q_PROPERTY(QStringList selectedCategories READ getSelectedCategories NOTIFY fireCategoriesChanged)

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

		[[nodiscard]] QStringList getLevels() const;
		[[nodiscard]] QStringList getSelectedLevels() const;
		[[nodiscard]] QStringList getCategories() const;
		[[nodiscard]] QStringList getSelectedCategories() const;

		Q_INVOKABLE void configureLevel(const QString& pLevel, bool pEnabled);
		Q_INVOKABLE void configureCategory(const QString& pCategory, bool pEnabled);

	Q_SIGNALS:
		void fireLevelsChanged();
		void fireCategoriesChanged();
};

} // namespace governikus
