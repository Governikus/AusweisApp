/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ContentItem.h"
#include "DiagnosisAntivirusDetection.h"
#include "DiagnosisConnectionTest.h"
#include "DiagnosisFirewallDetection.h"
#include "SectionModel.h"
#include "context/DiagnosisContext.h"

#include <QAbstractListModel>
#include <QMap>
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

		enum class Section : int
		{
			GENERAL,
			READER,
			NETWORK,
			SECURITY
		};

		QMap<Section, QSharedPointer<SectionModel>> mSections;
		QSharedPointer<DiagnosisContext> mContext;

		QVector<ContentItem> mAusweisApp2Section;
		QVector<ContentItem> mTimestampSection;

		bool mRemoteDeviceSectionRunning;
		QVector<ContentItem> mRemoteDeviceSection;
		bool mCardReaderSectionRunning;
		QVector<ContentItem> mCardReaderSection;
		bool mPcscSectionRunning;
		QVector<ContentItem> mPcscSection;

		DiagnosisConnectionTest mConnectionTest;
		QVector<ContentItem> mNetworkConnectionSection;
		QVector<ContentItem> mNetworkInterfaceSection;

		bool mAntivirusSectionRunning;
		DiagnosisAntivirusDetection mAntivirusDetection;
		QVector<ContentItem> mAntivirusSection;
		bool mFirewallSectionRunning;
		DiagnosisFirewallDetection mFirewallDetection;
		QVector<ContentItem> mFirewallSection;

		[[nodiscard]] QString getSectionName(Section pSection) const;
		void initGeneralSections();
		void updateGeneralSection();
		void initNetworkSections();
		void updateNetworkSection(bool pUpdateTimestamp = true);
		void initCardReaderSections();
		void updateCardReaderSection(bool pUpdateTimestamp = true);
		void initAntiVirusAndFirewallSection();
		void updateAntiVirusAndFirewallSection(bool pUpdateTimestamp = true);
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
