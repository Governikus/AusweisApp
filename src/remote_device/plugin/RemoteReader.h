/*!
 * \brief Implementation of \ref Reader for remote reader.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
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

		virtual CardEvent updateCard() override;

	public:
		RemoteReader(const QString& pReaderName, const QSharedPointer<RemoteDispatcherClient>& pRemoteDispatcher, const IfdStatus& pIfdStatus);
		virtual ~RemoteReader() override;

		virtual Card* getCard() const override;

		void update(const IfdStatus& pIfdStatus);
};

} // namespace governikus
