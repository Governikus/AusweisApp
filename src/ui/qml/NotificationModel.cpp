/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

#include "NotificationModel.h"

#include "LogHandler.h"
#include "SingletonHelper.h"

#include <QTime>


using namespace governikus;


defineSingleton(NotificationModel)


QString NotificationModel::getLastType() const
{
	if (mNotificationEntries.isEmpty())
	{
		return QString();
	}

	return qAsConst(mNotificationEntries).last().mType;
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

		beginInsertRows(QModelIndex(), mNotificationEntries.size(), mNotificationEntries.size());
		const auto& time = QTime::currentTime().toString(tr("hh:mm:ss"));
		mNotificationEntries.append({pCategoryName, time, pMsg});
		endInsertRows();

		Q_EMIT fireLastTypeChanged();
	}
}


NotificationModel::NotificationModel()
	: QAbstractListModel()
	, mNotificationEntries(20)
{
	connect(Env::getSingleton<LogHandler>(), &LogHandler::fireRawLog, this, &NotificationModel::onNewLogMsg, Qt::QueuedConnection);
}


NotificationModel& NotificationModel::getInstance()
{
	return *Instance;
}


int NotificationModel::rowCount(const QModelIndex& pIndex) const
{
	Q_UNUSED(pIndex)
	return mNotificationEntries.count();
}


QVariant NotificationModel::data(const QModelIndex& pIndex, int pRole) const
{
	if (mNotificationEntries.isEmpty() || pIndex.row() >= mNotificationEntries.size())
	{
		return QVariant();
	}
	const auto& notification = qAsConst(mNotificationEntries).at(mNotificationEntries.firstIndex() + pIndex.row());
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
