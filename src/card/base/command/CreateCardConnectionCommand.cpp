/*!
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#include "CreateCardConnectionCommand.h"

#include "Initializer.h"
#include "ReaderManagerWorker.h"

#include <QLoggingCategory>
#include <QThread>

Q_DECLARE_LOGGING_CATEGORY(card)

using namespace governikus;


static Initializer::Entry X([] {
			qRegisterMetaType<QSharedPointer<CreateCardConnectionCommand> >("QSharedPointer<CreateCardConnectionCommand>");
		});


CreateCardConnectionCommand::CreateCardConnectionCommand(const QString& pReaderName, const QPointer<ReaderManagerWorker>& pReaderManagerWorker)
	: QObject()
	, mReaderName(pReaderName)
	, mReaderManagerWorker(pReaderManagerWorker)
	, mCardConnection(nullptr)
{
}


void CreateCardConnectionCommand::run()
{
	QMetaObject::invokeMethod(this, &CreateCardConnectionCommand::execute, Qt::QueuedConnection);
}


void CreateCardConnectionCommand::execute()
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	if (connect(mReaderManagerWorker.data(), &ReaderManagerWorker::fireCardConnectionWorkerCreated, this, &CreateCardConnectionCommand::onCardConnectionWorkerCreated, Qt::UniqueConnection))
	{
		const auto& localCopy = mReaderManagerWorker;
		const auto& name = mReaderName;
		QMetaObject::invokeMethod(localCopy.data(), [localCopy, name] {
					localCopy->createCardConnectionWorker(name);
				}, Qt::QueuedConnection);
	}
	else
	{
		qCCritical(card) << "Cannot invoke ReaderManagerWorker to create CardConnectionWorker";
		QSharedPointer<CreateCardConnectionCommand> command(this, &QObject::deleteLater);
		Q_EMIT fireCommandDone(command);
	}
}


void CreateCardConnectionCommand::onCardConnectionWorkerCreated(QSharedPointer<CardConnectionWorker> pWorker)
{
	if (pWorker != nullptr)
	{
		mCardConnection.reset(new CardConnection(pWorker));
	}
	QSharedPointer<CreateCardConnectionCommand> command(this, &QObject::deleteLater);
	Q_EMIT fireCommandDone(command);
}


QSharedPointer<CardConnection> CreateCardConnectionCommand::getCardConnection() const
{
	return mCardConnection;
}


const QString& CreateCardConnectionCommand::getReaderName() const
{
	return mReaderName;
}
