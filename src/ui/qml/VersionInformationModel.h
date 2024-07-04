/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "SingletonCreator.h"

#include <QAbstractListModel>
#include <QList>
#include <QPair>
#include <QString>
#include <QtQml/qqmlregistration.h>


namespace governikus
{

class VersionInformationModel
	: public QAbstractListModel
	, public SingletonCreator<VersionInformationModel>
{
	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON

	friend class Env;

	private:
		enum VersionRoles
		{
			LABEL = Qt::UserRole + 1,
			TEXT
		};
		QList<QPair<QString, QString>> mData;

		VersionInformationModel();
		~VersionInformationModel() override = default;

		void init();

	public:
		[[nodiscard]] int rowCount(const QModelIndex& = QModelIndex()) const override;
		[[nodiscard]] QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		[[nodiscard]] QHash<int, QByteArray> roleNames() const override;
};

} // namespace governikus
