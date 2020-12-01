/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/DiagnosisContext.h"
#include "ContentItem.h"
#include "DiagnosisAntivirusDetection.h"
#include "DiagnosisConnectionTest.h"
#include "DiagnosisFirewallDetection.h"
#include "SectionModel.h"

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

	private:
		friend class ::test_DiagnosisModel;
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

	public:
		explicit DiagnosisModel(const QSharedPointer<DiagnosisContext>& pContext);
		~DiagnosisModel() override;

		QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		int rowCount(const QModelIndex& pParent = QModelIndex()) const override;

		QString getFirstSectionName() const;
		QAbstractListModel* getSectionContent(const QString& pSection);
		QDateTime getCreationTime() const;
		Q_INVOKABLE QString getCreationTimeString() const;
		QString getAsPlaintext() const;
		QString boolToString(bool pBoolean) const;
		bool isRunning() const;

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
		void reloadContent();
};

} // namespace governikus
