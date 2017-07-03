/*!
 * \brief Model implementation for the providers.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "ProviderSettings.h"

#include <QAbstractListModel>
#include <QPointer>


class test_ProviderModel;


namespace governikus
{

class ProviderModel
	: public QAbstractListModel
{
	friend class::test_ProviderModel;

	Q_OBJECT

	QPointer<ProviderSettings> mSettings;

	static QString createCostString(double pCostsPerMinute, double pCostsPerCall);
	static QString createAmountString(double pCents);

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

		ProviderModel(ProviderSettings* pSettings, QObject* pParent = nullptr);
		virtual ~ProviderModel();

		int rowCount(const QModelIndex&) const override;
		QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		QHash<int, QByteArray> roleNames() const override;

		static QString createCostString(const CallCost& pCosts);
};


} /* namespace governikus */
