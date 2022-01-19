/*!
 * \brief Implementation of \ref Reader for iOS.
 *
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IosCard.h"
#include "Reader.h"

#include <QObject>
#include <QScopedPointer>

#include <os/availability.h>

Q_FORWARD_DECLARE_OBJC_CLASS(IosReaderDelegate);

namespace governikus
{

struct IosCardPointer;

class IosReader
	: public ConnectableReader
{
	Q_OBJECT

	private:
		IosReaderDelegate* mDelegate API_AVAILABLE(ios(13.0)) = nullptr;
		QScopedPointer<IosCard, QScopedPointerDeleteLater> mCard;
		bool mConnected;
		bool mIsRestarting;

		virtual CardEvent updateCard() override;

		void removeCard();
		void startSession();
		void stopSession(const QString& pError);

	public:
		IosReader();
		virtual ~IosReader() override;

		virtual Card* getCard() const override;

		virtual void connectReader() override;
		virtual void disconnectReader(const QString& pError = QString()) override;

	Q_SIGNALS:
		void fireTagDiscovered(IosCardPointer* tag);
		void fireDidInvalidateWithError(bool doRestart);

	private Q_SLOTS:
		void onTagDiscovered(IosCardPointer* pCard);
		void onDidInvalidateWithError(bool pDoRestart);
};

} // namespace governikus
