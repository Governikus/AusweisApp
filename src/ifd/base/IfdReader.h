/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Implementation of \ref Reader for smartphone as card reader (SaC).
 */

#pragma once

#include "IfdCard.h"
#include "IfdDispatcherClient.h"
#include "Reader.h"
#include "messages/IfdStatus.h"

#include <QScopedPointer>
#include <QSharedPointer>


namespace governikus
{

class IfdReader
	: public Reader
{
	Q_OBJECT

	private:
		QScopedPointer<IfdCard, QScopedPointerDeleteLater> mCard;
		const QSharedPointer<IfdDispatcherClient> mDispatcher;

	public:
		IfdReader(ReaderManagerPluginType pPluginType, const QString& pReaderName, const QSharedPointer<IfdDispatcherClient>& pDispatcher, const IfdStatus& pIfdStatus);
		~IfdReader() override;

		[[nodiscard]] Card* getCard() const override;

		void updateStatus(const IfdStatus& pIfdStatus);
};

} // namespace governikus
