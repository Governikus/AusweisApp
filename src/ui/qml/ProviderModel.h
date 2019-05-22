/*!
 * \brief Model implementation for the providers.
 *
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CallCost.h"

#include <QAbstractListModel>
#include <QVector>


class test_ProviderModel;


namespace governikus
{

class ProviderModel
	: public QAbstractListModel
{
	friend class ::test_ProviderModel;

	Q_OBJECT

	static QString createCostString(double pCostsPerMinute, double pCostsPerCall);
	static QString createAmountString(double pCents);

	private:
		QVector<QMetaObject::Connection> mConnections;

		void updateConnections();

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
			PHONE_COST,
			EMAIL,
			POSTALADDRESS,
			ICON,
			IMAGE,
			SORT_ROLE
		};

		ProviderModel(QObject* pParent = nullptr);
		virtual ~ProviderModel() override;

		int rowCount(const QModelIndex&) const override;
		QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		QHash<int, QByteArray> roleNames() const override;

		static QString createCostString(const CallCost& pCosts);
};


} // namespace governikus
