/*!
 * \brief Implementation of \ref Reader for iOS.
 *
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IosCard.h"
#include "IosReaderDelegate.h"
#include "Reader.h"

#include <QObject>
#include <QScopedPointer>


namespace governikus
{

class IosReader
	: public ConnectableReader
{
	Q_OBJECT

	private:
		IosReaderDelegate mDelegate;
		QScopedPointer<IosCard, QScopedPointerDeleteLater> mCard;
		bool mConnected;
		qint64 mLastRestart;

		virtual CardEvent updateCard() override;

		void removeCard();
		void stopSession(const QString& pError);

	public:
		IosReader();
		virtual ~IosReader() override;

		virtual Card* getCard() const override;

		virtual void connectReader() override;
		virtual void disconnectReader(const QString& pError = QString()) override;

	private Q_SLOTS:
		void onDiscoveredTag(IosCard* pCard);
		void onDidInvalidateWithError(const QString& pError, bool pDoRestart);
		void onConnectFailed();
		void onTransmitFailed();
};

} // namespace governikus
