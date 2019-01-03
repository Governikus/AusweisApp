/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Card.h"
#include "ReaderInfo.h"

#include <QObject>
#include <QSharedPointer>
#include <QTimerEvent>

class test_Reader;

namespace governikus
{

class Reader
	: public QObject
{
	Q_OBJECT

	protected:
		enum class CardEvent
		{
			NONE, CARD_INSERTED, CARD_REMOVED,
		};

		ReaderInfo mReaderInfo;
		int mTimerId;

		void timerEvent(QTimerEvent* pEvent) override;

	private:
		friend class ::test_Reader;
		virtual CardEvent updateCard() = 0;

		CardReturnCode getRetryCounter(QSharedPointer<CardConnectionWorker> pCardConnectionWorker, int& pRetryCounter, bool& pPinDeactivated);

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
		virtual void disconnectReader() = 0;
};

} // namespace governikus
