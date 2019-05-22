/*!
 * \brief Model implementation for the history entries.
 *
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "HistoryModelSearchFilter.h"
#include "HistorySettings.h"
#include "ProviderConfigurationInfo.h"

#include <QAbstractListModel>
#include <QPointer>
#include <QSortFilterProxyModel>

namespace governikus
{

class HistoryProxyModel
	: public QSortFilterProxyModel
{
	Q_OBJECT

	public:
		Q_INVOKABLE bool removeRows(int pRow, int pCount, const QModelIndex& pParent = QModelIndex()) override;

		HistoryProxyModel();

		virtual ~HistoryProxyModel() override;
};


class ProviderNameFilterModel
	: public QSortFilterProxyModel
{
	Q_OBJECT

	private:
		QPointer<HistorySettings> mHistorySettings;

		ProviderConfigurationInfo mProvider;

	protected:
		bool filterAcceptsRow(int pSourceRow, const QModelIndex& pSourceParent) const override;

	public:
		ProviderNameFilterModel(HistorySettings* pHistorySettings);

		virtual ~ProviderNameFilterModel() override;

		Q_INVOKABLE void setProviderAddress(const QString& pProviderAddress);

};


class HistoryModel
	: public QAbstractListModel
{
	Q_OBJECT
	Q_PROPERTY(HistoryProxyModel * filter READ getFilterModel CONSTANT)
	Q_PROPERTY(ProviderNameFilterModel * nameFilter READ getNameFilterModel CONSTANT)
	Q_PROPERTY(HistoryModelSearchFilter * searchFilter READ getHistoryModelSearchFilter CONSTANT)
	Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY fireEnabledChanged)

	QPointer<HistorySettings> mHistorySettings;
	HistoryProxyModel mFilterModel;
	ProviderNameFilterModel mNameFilterModel;
	HistoryModelSearchFilter mHistoryModelSearchFilter;

	private:
		QVector<QMetaObject::Connection> mConnections;

		ProviderConfigurationInfo determineProviderFor(const HistoryInfo& pHistoryInfo) const;

		bool isEnabled() const;
		void setEnabled(bool pEnabled);
		void updateConnections();

	private Q_SLOTS:
		void onHistoryEntriesChanged();
		void onProvidersChanged();

	Q_SIGNALS:
		void fireEnabledChanged(bool pValue);

	public:
		HistoryModel(HistorySettings* pHistorySettings, QObject* pParent = nullptr);
		virtual ~HistoryModel() override;

		enum HistoryRoles
		{
			SUBJECT = Qt::UserRole + 1,
			PURPOSE,
			DATETIME,
			TERMSOFUSAGE,
			REQUESTEDDATA,
			PROVIDER_CATEGORY,
			PROVIDER_SHORTNAME,
			PROVIDER_LONGNAME,
			PROVIDER_SHORTDESCRIPTION,
			PROVIDER_LONGDESCRIPTION,
			PROVIDER_ADDRESS,
			PROVIDER_ADDRESS_DOMAIN,
			PROVIDER_HOMEPAGE,
			PROVIDER_HOMEPAGE_BASE,
			PROVIDER_PHONE,
			PROVIDER_PHONE_COST,
			PROVIDER_EMAIL,
			PROVIDER_POSTALADDRESS,
			PROVIDER_ICON,
			PROVIDER_IMAGE
		};

		int rowCount(const QModelIndex& = QModelIndex()) const override;
		QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		QHash<int, QByteArray> roleNames() const override;
		Q_INVOKABLE bool removeRows(int pRow, int pCount, const QModelIndex& pParent = QModelIndex()) override;

		Q_INVOKABLE HistoryProxyModel* getFilterModel();
		Q_INVOKABLE ProviderNameFilterModel* getNameFilterModel();
		HistoryModelSearchFilter* getHistoryModelSearchFilter();
};

} // namespace governikus
