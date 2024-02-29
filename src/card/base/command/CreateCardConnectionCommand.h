/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Command implementation for asynchronous CardConnection creation
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
	friend class ::test_StateConnectCard;

	private:
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
