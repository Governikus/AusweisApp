/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ContentItem.h"
#include "DiagnosisAntivirusDetection.h"
#include "DiagnosisConnectionTest.h"
#include "DiagnosisFirewallDetection.h"
#include "SectionModel.h"
#include "context/DiagnosisContext.h"
#include "controller/DiagnosisController.h"

#include <QAbstractListModel>
#include <QList>
#include <QMap>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QString>
#include <QUrl>
#include <QtQml/qqmlregistration.h>


class test_DiagnosisModel;


namespace governikus
{

class DiagnosisModel
	: public QAbstractListModel
{
	Q_OBJECT
	QML_ELEMENT

	friend class ::test_DiagnosisModel;

	Q_PROPERTY(bool running READ isRunning NOTIFY fireRunningChanged)

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
		DiagnosisController mDiagnosisController;

		QList<ContentItem> mAusweisAppSection;
		QList<ContentItem> mTimestampSection;

		bool mRemoteDeviceSectionRunning;
		QList<ContentItem> mRemoteDeviceSection;
		bool mCardReaderSectionRunning;
		QList<ContentItem> mCardReaderSection;
		bool mPcscSectionRunning;
		QList<ContentItem> mPcscSection;

		DiagnosisConnectionTest mConnectionTest;
		QList<ContentItem> mNetworkConnectionSection;
		QList<ContentItem> mNetworkInterfaceSection;

		bool mAntivirusSectionRunning;
		DiagnosisAntivirusDetection mAntivirusDetection;
		QList<ContentItem> mAntivirusSection;
		bool mFirewallSectionRunning;
		DiagnosisFirewallDetection mFirewallDetection;
		QList<ContentItem> mFirewallSection;

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
		[[nodiscard]] QString getAsPlaintext() const;

		void initContent();

	public:
		explicit DiagnosisModel();
		~DiagnosisModel() override;

		[[nodiscard]] QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		[[nodiscard]] int rowCount(const QModelIndex& pParent = QModelIndex()) const override;
		[[nodiscard]] QHash<int, QByteArray> roleNames() const override;

		[[nodiscard]] Q_INVOKABLE QString getCreationTime() const;
		Q_INVOKABLE void saveToFile(const QUrl& pFilename) const;
		[[nodiscard]] bool isRunning() const;

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
