/*!
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ProviderConfigurationInfo.h"

#include <QPointer>
#include <QSortFilterProxyModel>

class test_ProviderNameFilterModel;

namespace governikus
{


class ProviderNameFilterModel
	: public QSortFilterProxyModel
{
	Q_OBJECT
	friend class ::test_ProviderNameFilterModel;

	private:
		ProviderConfigurationInfo mProvider;

	protected:
		bool filterAcceptsRow(int pSourceRow, const QModelIndex& pSourceParent) const override;

	public:
		ProviderNameFilterModel();


		Q_INVOKABLE void setProviderAddress(const QString& pProviderAddress);
};

} // namespace governikus
