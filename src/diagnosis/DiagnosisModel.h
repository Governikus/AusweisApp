/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ContentItem.h"
#include "DiagnosisAntivirusDetection.h"
#include "DiagnosisConnectionTest.h"
#include "DiagnosisFirewallDetection.h"
#include "SectionModel.h"
#include "context/DiagnosisContext.h"

#include <QAbstractListModel>
#include <QPair>
#include <QSharedPointer>
#include <QVector>

class test_DiagnosisModel;

namespace governikus
{

class DiagnosisModel
	: public QAbstractListModel
{
	Q_OBJECT
	friend class ::test_DiagnosisModel;

	private:
		enum ContentRoles
		{
			ContentRole = Qt::UserRole + 1
		};

		QVector<QPair<QString, QSharedPointer<SectionModel>>> mSections;
		QSharedPointer<DiagnosisContext> mContext;
		DiagnosisAntivirusDetection mAntivirusDetection;
		DiagnosisFirewallDetection mFirewallDetection;
		DiagnosisConnectionTest mConnectionTest;
		QSharedPointer<ContentItem> mTimestampItem;
		QSharedPointer<SectionModel> mNetworkInterfaceSection;
		QSharedPointer<SectionModel> mNetworkConnectionSection;
		QSharedPointer<SectionModel> mCombinedNetworkSection;
		QSharedPointer<SectionModel> mCombinedAntivirusFirewallSection;
		QSharedPointer<SectionModel> mAntivirusSection;
		bool mAntivirusSectionRunning;
		QSharedPointer<SectionModel> mFirewallSection;
		bool mFirewallSectionRunning;
		QSharedPointer<SectionModel> mCombinedReaderSection;
		QSharedPointer<SectionModel> mCardReaderSection;
		bool mCardReaderSectionRunning;
		QSharedPointer<SectionModel> mPcscSection;
		bool mPcscSectionRunning;
		QSharedPointer<SectionModel> mRemoteDeviceSection;
		bool mRemoteDeviceSectionRunning;

		QSharedPointer<SectionModel> createAusweisApp2Section();
		void createNetworkSection();
		void createCardReaderSection();
		void createAntiVirusAndFirewallSection();
		void connectSignals();
		void disconnectSignals();

		[[nodiscard]] QString boolToString(bool pBoolean) const;

	public:
		explicit DiagnosisModel(const QSharedPointer<DiagnosisContext>& pContext);
		~DiagnosisModel() override;

		[[nodiscard]] QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		[[nodiscard]] int rowCount(const QModelIndex& pParent = QModelIndex()) const override;
		[[nodiscard]] QHash<int, QByteArray> roleNames() const override;

		[[nodiscard]] QString getCreationTime() const;
		[[nodiscard]] QString getAsPlaintext() const;
		[[nodiscard]] bool isRunning() const;
		void reloadContent();

	Q_SIGNALS:
		void fireRunningChanged();

	private Q_SLOTS:
		void onTimestampChanged();
		void onNetworkInfoChanged();
		void onConnectionTestDone();
		void onAntivirusInformationChanged();
		void onAntivirusDetectionFailed();
		void onFirewallInformationReady();
		void onFirewallInformationFailed();
		void onPcscInfoChanged();
		void onRemoteInfosChanged();
		void onReaderInfosChanged();
};

} // namespace governikus
