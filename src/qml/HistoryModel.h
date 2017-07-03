/*!
 * \brief Model implementation for the history entries.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "HistorySettings.h"
#include "ProviderSettings.h"

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

		virtual ~HistoryProxyModel();
};


class ProviderNameFilterModel
	: public QSortFilterProxyModel
{
	Q_OBJECT

	private:
		QPointer<HistorySettings> mHistorySettings;

		QString mProviderAddressHost;

	protected:
		bool filterAcceptsRow(int pSourceRow, const QModelIndex& pSourceParent) const override;

	public:
		ProviderNameFilterModel(HistorySettings* pHistorySettings);

		virtual ~ProviderNameFilterModel();

		Q_INVOKABLE void setProviderAddress(const QString& pProviderAddress);

};


class HistoryModel
	: public QAbstractListModel
{
	Q_OBJECT
	Q_PROPERTY(HistoryProxyModel * filter READ getFilterModel CONSTANT)
	Q_PROPERTY(ProviderNameFilterModel * nameFilter READ getNameFilterModel CONSTANT)

	QPointer<HistorySettings> mHistorySettings;
	QPointer<ProviderSettings> mProviderSettings;
	HistoryProxyModel mFilterModel;
	ProviderNameFilterModel mNameFilterModel;

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
		PROVIDER_EMAIL,
		PROVIDER_POSTALADDRESS,
		PROVIDER_ICON,
		PROVIDER_IMAGE
	};

	private:
		Provider determineProviderFor(const HistoryEntry& pHistoryEntry) const;

	private Q_SLOTS:
		void onHistoryEntriesChanged();
		void onProvidersChanged();

	public:
		HistoryModel(HistorySettings* pHistorySettings, ProviderSettings* pProviderSettings, QObject* pParent = nullptr);
		virtual ~HistoryModel();

		int rowCount(const QModelIndex& = QModelIndex()) const override;
		QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		QHash<int, QByteArray> roleNames() const override;
		Q_INVOKABLE bool removeRows(int pRow, int pCount, const QModelIndex& pParent = QModelIndex()) override;

		Q_INVOKABLE HistoryProxyModel* getFilterModel();
		Q_INVOKABLE ProviderNameFilterModel* getNameFilterModel();
};

} /* namespace governikus */
