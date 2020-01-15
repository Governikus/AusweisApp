/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/DiagnosisContext.h"
#include "DiagnosisAntivirusDetection.h"
#include "DiagnosisConnectionTest.h"
#include "DiagnosisFirewallDetection.h"
#include "DiagnosisItem.h"

#include <QAbstractItemModel>
#include <QObject>
#include <QSharedPointer>
#include <QVector>

class test_DiagnosisTreeModel;

namespace governikus
{

class DiagnosisTreeModel
	: public QAbstractItemModel
{
	Q_OBJECT

	private:
		friend class ::test_DiagnosisTreeModel;
		QSharedPointer<DiagnosisContext> mContext;
		QSharedPointer<DiagnosisItem> mRootItem;
		QSharedPointer<DiagnosisItem> mAppVersionItem;
		QSharedPointer<DiagnosisItem> mOperatingSystemItem;
		QSharedPointer<DiagnosisItem> mReaderItem;
		QSharedPointer<DiagnosisItem> mPcScItem;
		QSharedPointer<DiagnosisItem> mPairedDevices;
		QSharedPointer<DiagnosisItem> mNetworkInterfaces;
		QSharedPointer<DiagnosisItem> mNetworkConnectionTest;
		QSharedPointer<DiagnosisItem> mInstalledAntivirus;
		QSharedPointer<DiagnosisItem> mWindowsFirewall;
		QSharedPointer<DiagnosisItem> mTimestampItem;
		DiagnosisAntivirusDetection mAntivirusDetection;
		DiagnosisFirewallDetection mFirewallDetection;
		DiagnosisConnectionTest mConnectionTest;

		void initAppVersionInfo();
		void insertPcScComponentList(const QVector<DiagnosisContext::ComponentInfo>& pComponents, const QSharedPointer<DiagnosisItem>& pParentItem);
		void removeChildItems(const QModelIndex& pIndex, const QSharedPointer<DiagnosisItem>& pParentItem);
		static const QString boolToString(bool pBoolean);

	private Q_SLOTS:
		void onReaderInfosChanged();
		void onPcscInfoChanged();
		void onTimestampChanged();
		void onNetworkInfoChanged();
		void onRemoteInfosChanged();
		void onAntivirusInformationChanged();
		void onAntivirusDetectionFailed();
		void onFirewallInformationReady();
		void onFirewallInformationFailed();
		void onConnectionTestDone();

	public:
		explicit DiagnosisTreeModel(const QSharedPointer<DiagnosisContext>& pContext);

		QVariant data(const QModelIndex& pIndex, int pRole) const override;
		QModelIndex index(int pRow, int pColumn, const QModelIndex& pParent = QModelIndex()) const override;
		QModelIndex parent(const QModelIndex& pIndex) const override;
		int rowCount(const QModelIndex& pParent) const override;
		int columnCount(const QModelIndex& pParent) const override;
		QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

		QDateTime getCreationTime() const;
		Q_INVOKABLE QString getCreationTimeString() const;
		QString getAsPlaintext() const;
};

} // namespace governikus
