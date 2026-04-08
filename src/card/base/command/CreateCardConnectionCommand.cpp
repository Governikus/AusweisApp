/**
 * Copyright (c) 2015-2026 Governikus GmbH & Co. KG, Germany
 */

#include "CreateCardConnectionCommand.h"

#include "Initializer.h"
#include "ReaderManagerWorker.h"

#include <QLoggingCategory>
#include <QThread>

Q_DECLARE_LOGGING_CATEGORY(card)

using namespace governikus;


INIT_FUNCTION([] {
			qRegisterMetaType<QSharedPointer<CreateCardConnectionCommand>>("QSharedPointer<CreateCardConnectionCommand>");
		})


CreateCardConnectionCommand::CreateCardConnectionCommand(const QString& pReaderName, const QPointer<ReaderManagerWorker>& pReaderManagerWorker)
	: QObject()
	, mReaderName(pReaderName)
	, mReaderManagerWorker(pReaderManagerWorker)
	, mCardConnectionWorker(nullptr)
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

	mCardConnectionWorker = mReaderManagerWorker->createCardConnectionWorker(mReaderName, [] (const QSharedPointer<CardConnectionWorker>& pWorker) -> QSharedPointer<CardConnectionWorker> {
				if (pWorker)
				{
					const auto application = pWorker->getReaderInfo().getCardInfo().getApplication();
					if (application != FileRef() && !pWorker->selectApplicationRoot(application))
					{
						return nullptr;
					}
				}

				return pWorker;
			});

	QSharedPointer<CreateCardConnectionCommand> command(this, &QObject::deleteLater);
	Q_EMIT fireCommandDone(command);
}


QSharedPointer<CardConnection> CreateCardConnectionCommand::getCardConnection()
{
	Q_ASSERT(QCoreApplication::instance()->thread() == QThread::currentThread());

	if (mCardConnectionWorker && !mCardConnection)
	{
		mCardConnection = QSharedPointer<CardConnection>::create(mCardConnectionWorker);
	}
	return mCardConnection;
}


const QString& CreateCardConnectionCommand::getReaderName() const
{
	return mReaderName;
}
