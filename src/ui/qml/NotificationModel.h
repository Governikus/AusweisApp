/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "SingletonCreator.h"

#include <QAbstractListModel>
#include <QContiguousCache>
#include <QHash>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QtQml/qqmlregistration.h>

class test_NotificationModel;

namespace governikus
{

class NotificationModel
	: public QAbstractListModel
	, public SingletonCreator<NotificationModel>
{
	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON

	friend class Env;
	friend class ::test_NotificationModel;

	private:
		enum UserRoles
		{
			TIME = Qt::UserRole + 1,
			TEXT
		};

		struct NotificationEntry
		{
			QString mTime;
			QString mText;
		};

		QContiguousCache<NotificationEntry> mNotificationEntries;

		NotificationModel();
		~NotificationModel() override = default;

	private Q_SLOTS:
		void onNewLogMsg(const QString& pMsg, const QString& pCategoryName);

	public:
		[[nodiscard]] int rowCount(const QModelIndex& pIndex) const override;
		[[nodiscard]] QVariant data(const QModelIndex& pIndex, int pRole) const override;
		[[nodiscard]] QHash<int, QByteArray> roleNames() const override;
};

} // namespace governikus
