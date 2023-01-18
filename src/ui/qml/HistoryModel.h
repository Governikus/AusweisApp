/*!
 * \brief Model implementation for the history entries.
 *
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "HistoryModelSearchFilter.h"
#include "HistoryProxyModel.h"
#include "HistorySettings.h"
#include "ProviderConfigurationInfo.h"
#include "ProviderNameFilterModel.h"

#include <QAbstractListModel>


class test_HistoryModel;

namespace governikus
{

class HistoryModel
	: public QAbstractListModel
{
	Q_OBJECT
	friend class ::test_HistoryModel;

	Q_PROPERTY(HistoryProxyModel * filter READ getFilterModel CONSTANT)
	Q_PROPERTY(ProviderNameFilterModel * nameFilter READ getNameFilterModel CONSTANT)
	Q_PROPERTY(HistoryModelSearchFilter * searchFilter READ getHistoryModelSearchFilter CONSTANT)
	Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY fireEnabledChanged)
	Q_PROPERTY(bool empty READ isEmpty NOTIFY fireEmptyChanged)

	private:
		HistoryProxyModel mFilterModel;
		ProviderNameFilterModel mNameFilterModel;
		HistoryModelSearchFilter mHistoryModelSearchFilter;
		QVector<QMetaObject::Connection> mConnections;

		ProviderConfigurationInfo determineProviderFor(const HistoryInfo& pHistoryInfo) const;
		static auto& getHistorySettings();

		[[nodiscard]] bool isEnabled() const;
		void setEnabled(bool pEnabled);
		[[nodiscard]] bool isEmpty() const;
		void updateConnections();

	private Q_SLOTS:
		void onHistoryEntriesChanged();
		void onProvidersChanged();

	Q_SIGNALS:
		void fireEnabledChanged(bool pValue);
		void fireEmptyChanged(bool pValue);

	public:
		explicit HistoryModel(QObject* pParent = nullptr);
		~HistoryModel() override = default;

		enum HistoryRoles
		{
			SUBJECT = Qt::UserRole + 1,
			PURPOSE,
			DATETIME,
			TERMSOFUSAGE,
			REQUESTEDDATA,
			WRITTENDATA,
			PROVIDER_CATEGORY,
			PROVIDER_SHORTNAME,
			PROVIDER_LONGNAME,
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

		[[nodiscard]] int rowCount(const QModelIndex& = QModelIndex()) const override;
		[[nodiscard]] QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		[[nodiscard]] QHash<int, QByteArray> roleNames() const override;
		[[nodiscard]] Q_INVOKABLE bool removeRows(int pRow, int pCount, const QModelIndex& pParent = QModelIndex()) override;

		[[nodiscard]] Q_INVOKABLE HistoryProxyModel* getFilterModel();
		[[nodiscard]] Q_INVOKABLE ProviderNameFilterModel* getNameFilterModel();
		[[nodiscard]] HistoryModelSearchFilter* getHistoryModelSearchFilter();

		Q_INVOKABLE void exportHistory(const QUrl& pFilename) const;

#ifndef QT_NO_DEBUG
		Q_INVOKABLE void createDummyEntry();
#endif
};

} // namespace governikus
