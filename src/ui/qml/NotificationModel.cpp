/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */

#include "NotificationModel.h"

#include "LogHandler.h"

#include <QTime>

using namespace governikus;

NotificationModel::NotificationModel()
	: QAbstractListModel()
	, mNotificationEntries(20)
{
	connect(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireRawLog, this, &NotificationModel::onNewLogMsg, Qt::QueuedConnection);
}


QString NotificationModel::getLastType() const
{
	if (mNotificationEntries.isEmpty())
	{
		return QString();
	}

	return std::as_const(mNotificationEntries).last().mType;
}


void NotificationModel::onNewLogMsg(const QString& pMsg, const QString& pCategoryName)
{
	if (pCategoryName == QLatin1String("developermode") || pCategoryName == QLatin1String("feedback"))
	{
		if (mNotificationEntries.isFull())
		{
			beginRemoveRows(QModelIndex(), 0, 0);
			mNotificationEntries.removeFirst();
			endRemoveRows();
		}

		const auto size = static_cast<int>(mNotificationEntries.size());
		beginInsertRows(QModelIndex(), size, size);
		//: LABEL ALL_PLATFORMS Time format according to https://doc.qt.io/qt/qtime.html#toString
		const auto& time = QTime::currentTime().toString(tr("hh:mm:ss"));
		mNotificationEntries.append({pCategoryName, time, pMsg});
		endInsertRows();

		Q_EMIT fireLastTypeChanged();
	}
}


int NotificationModel::rowCount(const QModelIndex& pIndex) const
{
	Q_UNUSED(pIndex)
	return static_cast<int>(mNotificationEntries.size());
}


QVariant NotificationModel::data(const QModelIndex& pIndex, int pRole) const
{
	if (mNotificationEntries.isEmpty() || pIndex.row() >= mNotificationEntries.size())
	{
		return QVariant();
	}
	const auto& notification = std::as_const(mNotificationEntries).at(mNotificationEntries.firstIndex() + pIndex.row());
	switch (pRole)
	{
		case TYPE:
			return notification.mType;

		case TIME:
			return notification.mTime;

		default:
			return notification.mText;
	}
}


QHash<int, QByteArray> NotificationModel::roleNames() const
{
	QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
	roles.insert(TYPE, "type");
	roles.insert(TIME, "time");
	roles.insert(TEXT, "text");
	return roles;
}
