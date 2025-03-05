/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "ReaderConfigurationInfo.h"
#include "SingletonCreator.h"
#include "SortedReaderModel.h"

#include <QAbstractTableModel>
#include <QHash>
#include <QList>
#include <QSet>
#include <QTime>
#include <QtQml/qqmlregistration.h>

class test_ReaderModel;

namespace governikus
{

class ReaderModel
	: public QAbstractListModel
	, public SingletonCreator<ReaderModel>
{
	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON

	friend class Env;
	friend class ::test_ReaderModel;

	Q_PROPERTY(QString lastUpdatedInformation READ getLastUpdatedInformation NOTIFY fireModelChanged)
	Q_PROPERTY(governikus::SortedReaderModel * sortedModel READ getSortedModel CONSTANT)

	private:
		ReaderModel();

		QSet<const ReaderConfigurationInfo> mKnownDrivers;
		QList<ReaderConfigurationInfo> mConnectedReaders;
		QTime mConnectedReadersUpdateTime;
		SortedReaderModel mSortedModel;

		[[nodiscard]] QString getLastUpdatedInformation() const;
		[[nodiscard]] SortedReaderModel* getSortedModel();

		void collectReaderData();
		[[nodiscard]] bool indexIsValid(const QModelIndex& pIndex) const;
		[[nodiscard]] QUrl getReaderImageUrl(const QModelIndex& pIndex) const;
		[[nodiscard]] QString getHTMLDescription(const QModelIndex& pIndex) const;
		[[nodiscard]] bool isSupportedReader(const QModelIndex& pIndex) const;
		[[nodiscard]] bool isInstalledReader(const QModelIndex& pIndex) const;
		[[nodiscard]] bool isPcscScanRunning() const;

	private Q_SLOTS:
		void onUpdateContent();

	public:
		enum UserRoles
		{
			READER_NAME = Qt::UserRole + 1,
			READER_STATUS,
			READER_IMAGE_PATH,
			READER_HTML_DESCRIPTION,
			READER_DRIVER_URL,
			READER_INSTALLED,
			READER_SUPPORTED
		};

		[[nodiscard]] int rowCount(const QModelIndex& pParent = QModelIndex()) const override;
		[[nodiscard]] QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		[[nodiscard]] QHash<int, QByteArray> roleNames() const override;

	public Q_SLOTS:
		void onTranslationChanged();

	Q_SIGNALS:
		void fireModelChanged();
};


} // namespace governikus
