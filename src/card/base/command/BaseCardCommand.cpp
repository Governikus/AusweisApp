/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "BaseCardCommand.h"

#include "asn1/SecurityInfos.h"
#include "CardConnection.h"
#include "Initializer.h"

#include <QLoggingCategory>
#include <QSharedPointer>
#include <QThread>

Q_DECLARE_LOGGING_CATEGORY(card)

using namespace governikus;

INIT_FUNCTION([] {
			qRegisterMetaType<QSharedPointer<BaseCardCommand> >("QSharedPointer<BaseCardCommand>");
		})


BaseCardCommand::BaseCardCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker)
	: mCardConnectionWorker(pCardConnectionWorker)
	, mReturnCode(CardReturnCode::UNKNOWN)
{
	Q_ASSERT(mCardConnectionWorker);
}


BaseCardCommand::~BaseCardCommand()
{
}


void BaseCardCommand::run()
{
	QMetaObject::invokeMethod(this, &BaseCardCommand::execute, Qt::QueuedConnection);
}


void BaseCardCommand::execute()
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	internalExecute();
	qCDebug(card) << metaObject()->className() << "| ReturnCode of internal execute:" << mReturnCode;

	// A "Command" is created by CardConnection::call() in Main-Thread and moved to ReaderManager-Thread.
	// The internal execution of a command will be self-sufficient until it has finished. After the
	// command is finished it is a data container only. It will fires a signal with itself wrapped into a
	// QSharedPointer to ensure the destruction in correct thread. This structure is used to have an
	// easy-to-use API of commands without knowledge about threads.
	QSharedPointer<BaseCardCommand> command(this, &QObject::deleteLater);
	Q_EMIT commandDone(command);
}
