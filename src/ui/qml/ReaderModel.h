/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Model implementation for recognized readers
 */

#pragma once

#include "ReaderConfigurationInfo.h"
#include "SortedReaderModel.h"

#include <QAbstractTableModel>
#include <QHash>
#include <QSet>
#include <QTime>
#include <QVector>


namespace governikus
{

class ReaderModel
	: public QAbstractListModel
{
	Q_OBJECT

	Q_PROPERTY(QString lastUpdatedInformation READ getLastUpdatedInformation NOTIFY fireModelChanged)
	Q_PROPERTY(SortedReaderModel * sortedModel READ getSortedModel CONSTANT)

	private:
		QSet<const ReaderConfigurationInfo> mKnownDrivers;
		QVector<ReaderConfigurationInfo> mConnectedReaders;
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
		explicit ReaderModel(QObject* pParent = nullptr);

		[[nodiscard]] int rowCount(const QModelIndex& pParent = QModelIndex()) const override;
		[[nodiscard]] QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		[[nodiscard]] QHash<int, QByteArray> roleNames() const override;

	public Q_SLOTS:
		void onTranslationChanged();

	Q_SIGNALS:
		void fireModelChanged();
};


} // namespace governikus
