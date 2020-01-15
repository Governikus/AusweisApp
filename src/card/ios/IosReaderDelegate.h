/*!
 * \brief Implementation of \ref IosReaderDelegate for iOS.
 *
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IosCard.h"

#include <QObject>


namespace governikus
{

class IosReaderDelegate
	: public QObject
{
	Q_OBJECT
	class Private;

	IosReaderDelegate::Private* mPrivate;

	public:
		IosReaderDelegate();
		virtual ~IosReaderDelegate() override;

		void startSession();
		void stopSession(const QString& pMessage, bool pIsError = false);

		void onTagDiscovered(IosCard* pCard);
		void onDidInvalidateWithError(const QString& pError, bool pDoRestart);

	Q_SIGNALS:
		void fireDiscoveredTag(IosCard* pCard);
		void fireDidInvalidateWithError(const QString& pError, bool pDoRestart);


};

} // namespace governikus
