/*!
 * \brief Command implementation for asynchronous CardConnection creation
 *
 * \copyright Copyright (c) 2015-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CardConnection.h"

#include <QObject>
#include <QSharedPointer>
#include <QString>

class test_StateConnectCard;

namespace governikus
{

class ReaderManagerWorker;

class CreateCardConnectionCommand
	: public QObject
{
	Q_OBJECT

	private:
		friend class ::test_StateConnectCard;
		const QString mReaderName;
		QPointer<ReaderManagerWorker> mReaderManagerWorker;
		QSharedPointer<CardConnection> mCardConnection;

		Q_INVOKABLE void execute();

	public:
		explicit CreateCardConnectionCommand(const QString& pReaderName, const QPointer<ReaderManagerWorker>& pReaderManagerWorker);

		void run();
		[[nodiscard]] QSharedPointer<CardConnection> getCardConnection() const;
		[[nodiscard]] const QString& getReaderName() const;

	private Q_SLOTS:
		void onCardConnectionWorkerCreated(QSharedPointer<CardConnectionWorker> pCardConnectionWorker);

	Q_SIGNALS:
		void fireCommandDone(QSharedPointer<CreateCardConnectionCommand> pCommand);

};

} // namespace governikus
