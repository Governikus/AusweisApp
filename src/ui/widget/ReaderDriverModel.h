/*!
 * \brief Model implementation for the reader driver table
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ReaderDetector.h"

#include <QAbstractTableModel>
#include <QSet>
#include <QVector>


namespace governikus
{

class ReaderDriverModel
	: public QAbstractTableModel
{
	Q_OBJECT

	private:
		const int NUMBER_OF_COLUMNS = 2;

		QSet<const ReaderConfigurationInfo> mKnownDrivers;
		QVector<ReaderConfigurationInfo> mConnectedReaders;

		QString getStatus(const ReaderConfigurationInfo& pReaderConfigurationInfo) const;
		void collectReaderData();

	public:
		enum ColumnId : int
		{
			ReaderName = 0,
			ReaderStatus = 1
		};
		ReaderDriverModel(QObject* pParent = nullptr);

		virtual QVariant headerData(int pSection, Qt::Orientation pOrientation, int pRole) const override;
		virtual int rowCount(const QModelIndex& pParent = QModelIndex()) const override;
		virtual int columnCount(const QModelIndex& pParent = QModelIndex()) const override;
		virtual QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;

		const ReaderConfigurationInfo& getReaderConfigurationInfo(const QModelIndex& pIndex) const;
		QString getHTMLDescription(const QModelIndex& pIndex) const;
		bool isInstalledSupportedReader(const QModelIndex& pIndex) const;

	public Q_SLOTS:
		void onUpdateContent();

	Q_SIGNALS:
		void fireModelChanged();

};


} // namespace governikus
