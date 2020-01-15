/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Card.h"
#include "ReaderInfo.h"

#include <QObject>
#include <QSharedPointer>
#include <QTimerEvent>

namespace governikus
{

class Reader
	: public QObject
{
	Q_OBJECT

	public:
		enum class CardEvent
		{
			NONE, CARD_INSERTED, CARD_REMOVED,
		};

	protected:
		ReaderInfo mReaderInfo;
		int mTimerId;

		void timerEvent(QTimerEvent* pEvent) override;

	private:
		virtual CardEvent updateCard() = 0;

		struct RetryCounterResult
		{
			CardReturnCode cardReturnCode = CardReturnCode::COMMAND_FAILED;
			int retryCounter = -1;
			bool pinDeactivated = false;
		};
		RetryCounterResult getRetryCounter(QSharedPointer<CardConnectionWorker> pCardConnectionWorker);

		void fireUpdateSignal(CardEvent pCardEvent);

	public:
		Reader(ReaderManagerPlugInType pPlugInType, const QString& pReaderName);
		virtual ~Reader() override = default;

		/*!
		 * Periodically called to perform an update of the readers and cards state.
		 */
		void update();

		const QString& getName() const
		{
			return mReaderInfo.getName();
		}


		const ReaderInfo& getReaderInfo() const
		{
			return mReaderInfo;
		}


		virtual Card* getCard() const = 0;

		void setPukInoperative();

		/*!
		 * \brief Creates a new CardConnectionWorker if and only if there is a card in the reader which is not already exclusively connected.
		 * \return a new CardConnectionWorker
		 */
		QSharedPointer<CardConnectionWorker> createCardConnectionWorker();

		CardReturnCode updateRetryCounter(QSharedPointer<CardConnectionWorker> pCardConnectionWorker);

	Q_SIGNALS:
		void fireCardInserted(const QString& pReaderName);
		void fireCardRemoved(const QString& pReaderName);
		void fireCardRetryCounterChanged(const QString& pReaderName);
		void fireReaderPropertiesUpdated(const QString& pReaderName);
		void fireReaderDeviceError(GlobalStatus::Code pErrorCode);


};


class ConnectableReader
	: public Reader
{
	Q_OBJECT

	public:
		using Reader::Reader;
		virtual ~ConnectableReader() override = default;

		virtual void connectReader() = 0;
		virtual void disconnectReader(const QString& pError = QString()) = 0;

	Q_SIGNALS:
		void fireReaderDisconnected();
};

} // namespace governikus
