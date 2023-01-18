/*!
 * \copyright Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "ExecuteCommand.h"

#include <QThread>

using namespace governikus;


ExecuteCommand::ExecuteCommand(const std::function<QVariant()>& pFunc)
	: mFunc(pFunc)
{
}


void ExecuteCommand::run()
{
	QMetaObject::invokeMethod(this, &ExecuteCommand::execute, Qt::QueuedConnection);
}


void ExecuteCommand::execute()
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());
	Q_EMIT fireCommandDone(mFunc());
	disconnect();
	deleteLater();
}
