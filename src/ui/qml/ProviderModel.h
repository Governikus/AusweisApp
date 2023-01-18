/*!
 * \brief Model implementation for the providers.
 *
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CallCost.h"

#include <QAbstractListModel>
#include <QStringList>
#include <QVector>


namespace governikus
{

class ProviderModel
	: public QAbstractListModel
{
	Q_OBJECT

	static QString createCostString(double pCostsPerMinute, double pCostsPerCall);
	static QString createAmountString(double pCents);

	private:
		QVector<QMetaObject::Connection> mConnections;
		bool mIncludeCategories;

		void updateConnections();

	private Q_SLOTS:
		void onProvidersChanged();

	public:
		enum ProviderRoles
		{
			CATEGORY = Qt::UserRole + 1,
			SHORTNAME,
			LONGNAME,
			LONGDESCRIPTION,
			ADDRESS,
			ADDRESS_DOMAIN,
			HOMEPAGE,
			HOMEPAGE_BASE,
			PHONE,
			PHONE_COST,
			EMAIL,
			POSTALADDRESS,
			ICON,
			IMAGE,
			SORT_ROLE,
			TYPE
		};

		explicit ProviderModel(QObject* pParent = nullptr);
		~ProviderModel() override = default;

		[[nodiscard]] int rowCount(const QModelIndex&) const override;
		[[nodiscard]] QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		[[nodiscard]] QHash<int, QByteArray> roleNames() const override;

		void setIncludeCategories(bool pIncludeCategories);
		[[nodiscard]] bool getIncludeCategories() const;

		[[nodiscard]] static const QStringList& getProviderCategories();

		[[nodiscard]] static QString createCostString(const CallCost& pCosts);
};


} // namespace governikus
