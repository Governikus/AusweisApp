/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"

#include <QAbstractListModel>
#include <QContiguousCache>
#include <QHash>
#include <QObject>
#include <QString>
#include <QVariant>

class test_NotificationModel;

namespace governikus
{

class NotificationModel
	: public QAbstractListModel
{
	Q_OBJECT
	friend class Env;
	friend class ::test_NotificationModel;

	Q_PROPERTY(QString lastType READ getLastType NOTIFY fireLastTypeChanged)

	private:
		enum UserRoles
		{
			TYPE = Qt::UserRole + 1,
			TIME,
			TEXT
		};

		struct NotificationEntry
		{
			QString mType;
			QString mTime;
			QString mText;
		};

		QContiguousCache<NotificationEntry> mNotificationEntries;

		NotificationModel();
		~NotificationModel() override = default;
		QString getLastType() const;

	private Q_SLOTS:
		void onNewLogMsg(const QString& pMsg, const QString& pCategoryName);

	public:
		[[nodiscard]] int rowCount(const QModelIndex& pIndex) const override;
		[[nodiscard]] QVariant data(const QModelIndex& pIndex, int pRole) const override;
		[[nodiscard]] QHash<int, QByteArray> roleNames() const override;

	Q_SIGNALS:
		void fireLastTypeChanged();
};

} // namespace governikus
