/*!
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#include "CreateCardConnectionCommand.h"

#include "Initializer.h"
#include "ReaderManagerWorker.h"

#include <QThread>

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


void CreateCardConnectionCommand::execute()
{
	Q_ASSERT(thread() == QThread::currentThread());

	if (connect(mReaderManagerWorker.data(), &ReaderManagerWorker::fireCardConnectionWorkerCreated, this, &CreateCardConnectionCommand::onCardConnectionWorkerCreated, Qt::UniqueConnection))
	{
		QMetaObject::invokeMethod(mReaderManagerWorker.data(), "createCardConnectionWorker", Qt::QueuedConnection, Q_ARG(QString, mReaderName));
	}
	else
	{
		qCritical() << "Cannot invoke ReaderManagerWorker to create CardConnectionWorker";
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
