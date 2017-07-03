/*!
 * CreateCardConnectionCommand.h
 *
 * \brief Command implementation for asynchronous CardConnection creation
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "CardConnection.h"

#include <QObject>
#include <QSharedPointer>
#include <QString>


namespace governikus
{

class CardConnectionWorker;
class ReaderManagerWorker;


class CreateCardConnectionCommand
	: public QObject
{
	Q_OBJECT

	private:
		QString mReaderName;
		QPointer<ReaderManagerWorker> mReaderManagerWorker;
		QSharedPointer<CardConnection> mCardConnection;

	public:
		CreateCardConnectionCommand(const QString& pReaderName, const QPointer<ReaderManagerWorker>& pReaderManagerWorker);
		Q_INVOKABLE void execute();
		QSharedPointer<CardConnection> getCardConnection() const;

	private Q_SLOTS:
		void onCardConnectionWorkerCreated(QSharedPointer<CardConnectionWorker> pCardConnectionWorker);

	Q_SIGNALS:
		void fireCommandDone(QSharedPointer<CreateCardConnectionCommand> pCommand);

};

} /* namespace governikus */
