/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Card.h"
#include "ReaderInfo.h"

#include <QObject>
#include <QSharedPointer>
#include <QTimerEvent>

namespace governikus
{
class CardConnectionWorker;

class Reader
	: public QObject
{
	Q_OBJECT
	friend class MockReader;

	protected:
		void setInfoBasicReader(bool pBasicReader);
		void setInfoMaxApduLength(int pMaxApduLength);
		void setInfoCardInfo(const CardInfo& pCardInfo);
		void setCardInfoTagType(CardInfo::TagType pTagType);
		void removeCardInfo();
		void fetchCardInfo();

		[[nodiscard]] int getTimerId() const;
		void setTimerId(int pTimerId);

		void timerEvent(QTimerEvent* pEvent) override;

	private:
		ReaderInfo mReaderInfo;
		int mTimerId;

		struct RetryCounterResult
		{
			CardReturnCode cardReturnCode = CardReturnCode::COMMAND_FAILED;
			int retryCounter = -1;
			bool pinDeactivated = false;
			bool pinInitial = false;
		};
		RetryCounterResult getRetryCounter(QSharedPointer<CardConnectionWorker> pCardConnectionWorker);

	public:
		Reader(ReaderManagerPluginType pPluginType, const QString& pReaderName);
		~Reader() override = default;


		virtual void printGetReaderInfo() const
		{
		}


		/*!
		 * Periodically called to perform an update of the readers and cards state.
		 */
		virtual void updateCard()
		{
		}


		[[nodiscard]] const QString& getName() const
		{
			return mReaderInfo.getName();
		}


		[[nodiscard]] const ReaderInfo& getReaderInfo() const
		{
			return mReaderInfo;
		}


		virtual void insertCard(const QVariant& pData = QVariant());

		void shelveCard();

		[[nodiscard]] virtual Card* getCard() const = 0;

		void setPukInoperative();

		/*!
		 * \brief Creates a new CardConnectionWorker if and only if there is a card in the reader which is not already exclusively connected.
		 * \return a new CardConnectionWorker
		 */
		QSharedPointer<CardConnectionWorker> createCardConnectionWorker();

		CardReturnCode updateRetryCounter(QSharedPointer<CardConnectionWorker> pCardConnectionWorker);

	Q_SIGNALS:
		void fireCardInserted(const ReaderInfo& pInfo);
		void fireCardRemoved(const ReaderInfo& pInfo);
		void fireCardInfoChanged(const ReaderInfo& pInfo);
		void fireReaderPropertiesUpdated(const ReaderInfo& pInfo);
};


class ConnectableReader
	: public Reader
{
	Q_OBJECT

	public:
		using Reader::Reader;
		~ConnectableReader() override = default;

		virtual void connectReader() = 0;
		virtual void disconnectReader(const QString& pError = QString()) = 0;

	Q_SIGNALS:
		void fireReaderDisconnected();
};

} // namespace governikus
