/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteDispatcher.h"

#include "Initializer.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


static Initializer::Entry E([] {
			qRegisterMetaType<QSharedPointer<RemoteDispatcher> >("QSharedPointer<RemoteDispatcher>");
		});


RemoteDispatcher::RemoteDispatcher()
	: QObject()
	, QEnableSharedFromThis()
{
}


RemoteDispatcher::~RemoteDispatcher()
{
}
