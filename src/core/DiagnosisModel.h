/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/DiagnosisContext.h"
#include "DiagnosisAntivirusDetection.h"
#include "DiagnosisConnectionTest.h"
#include "DiagnosisFirewallDetection.h"
#include "DiagnosisItem.h"

#include <QAbstractListModel>
#include <QPair>
#include <QSharedPointer>
#include <QVector>

class test_DiagnosisModel;

namespace governikus
{

class SectionModel;
struct ContentItem
{
	ContentItem(const QString& pTitle, const QString& pContent)
		: mTitle(pTitle)
		, mContent(pContent)
	{
	}


	QString mTitle;
	QString mContent;
	QSharedPointer<SectionModel> mSection;
};


class SectionModel
	: public QAbstractListModel
	, public QEnableSharedFromThis<SectionModel>
{
	Q_OBJECT

	enum ContentRoles
	{
		TitleRole = Qt::UserRole + 1,
		ContentRole
	};

	private:
		friend class ::test_DiagnosisModel;
		QVector<QSharedPointer<ContentItem> > mContentItems;

	public:
		explicit SectionModel(QObject* pParent = nullptr);

		QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		int rowCount(const QModelIndex& pParent = QModelIndex()) const override;
		QHash<int, QByteArray> roleNames() const override;

		void addItem(const QString& pTitle, const QString& pContent);
		void addItem(const QSharedPointer<ContentItem>& pContentItem);
		void addItemWithoutTitle(const QString& pContent);
		void addTitleWithoutContent(const QString& pTitle);
		void removeAllItems();
		void emitDataChangedForItem(const QSharedPointer<ContentItem>& pItem);
		void replaceWithSections(QVector<QSharedPointer<SectionModel> > pSections);
		QStringList getAsPlaintext(const QString& pPrependString = QString()) const;
};


class DiagnosisModel
	: public QAbstractListModel
{
	Q_OBJECT

	private:
		friend class ::test_DiagnosisModel;
		QVector<QPair<QString, QSharedPointer<SectionModel> > > mSections;
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
		QSharedPointer<SectionModel> mFirewallSection;
		QSharedPointer<SectionModel> mCombinedReaderSection;
		QSharedPointer<SectionModel> mCardReaderSection;
		QSharedPointer<SectionModel> mPcscSection;
		QSharedPointer<SectionModel> mRemoteDeviceSection;

		QSharedPointer<SectionModel> createAusweisApp2Section();
		void createNetworkSection();
		void createCardReaderSection();
		void createAntiVirusAndFirewallSection();
		void emitDataChangedForSection(const QSharedPointer<ContentItem>& pItem) const;
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
		QString boolToString(bool pBoolean);

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
