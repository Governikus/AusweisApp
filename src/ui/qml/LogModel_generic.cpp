/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "LogModel.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(qml)


using namespace governikus;


void LogModel::mailLog(const QString& pEmail, const QString& pSubject, const QString& pMsg)
{
	Q_UNUSED(pEmail);
	Q_UNUSED(pSubject);
	Q_UNUSED(pMsg);

	qCWarning(qml) << "NOT IMPLEMENTED";
}


void LogModel::shareLog()
{
	qCWarning(qml) << "NOT IMPLEMENTED";
}
