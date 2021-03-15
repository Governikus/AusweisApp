/*!
 * \brief Implementation of \ref Reader for smartphone as card reader (SaC).
 *
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "messages/IfdStatus.h"
#include "Reader.h"
#include "RemoteCard.h"
#include "RemoteDispatcherClient.h"

#include <QScopedPointer>
#include <QSharedPointer>


namespace governikus
{

class RemoteReader
	: public Reader
{
	Q_OBJECT

	private:
		QScopedPointer<RemoteCard, QScopedPointerDeleteLater> mCard;
		const QSharedPointer<RemoteDispatcherClient> mRemoteDispatcher;

		CardEvent updateCard() override;

	public:
		RemoteReader(const QString& pReaderName, const QSharedPointer<RemoteDispatcherClient>& pRemoteDispatcher, const IfdStatus& pIfdStatus);
		~RemoteReader() override;

		[[nodiscard]] Card* getCard() const override;

		void updateStatus(const IfdStatus& pIfdStatus);
};

} // namespace governikus
