/*!
 * \copyright Copyright (c) 2016-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QPair>
#include <QString>

namespace governikus
{

class VersionInformationModel
	: public QAbstractListModel
{
	Q_OBJECT

	private:
		enum HistoryRoles
		{
			LABEL = Qt::UserRole + 1,
			TEXT
		};
		QVector<QPair<QString, QString> > mData;

		void init();

	public:
		VersionInformationModel(QObject* pParent = nullptr);

		int rowCount(const QModelIndex& = QModelIndex()) const override;
		QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		QHash<int, QByteArray> roleNames() const override;
};

}
