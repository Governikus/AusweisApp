/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "paos/PaosType.h"

#include <QXmlStreamAttributes>

class test_PaosMessage;

namespace governikus
{

class PaosMessage
{
	friend class ::test_PaosMessage;

	private:
		QString mMessageID;
		QString mRelatesTo;

	public:
		const PaosType mType;

		explicit PaosMessage(PaosType pType);
		virtual ~PaosMessage();

		[[nodiscard]] const QString& getMessageId() const;
		void setMessageId(const QString& messageId);
		[[nodiscard]] const QString& getRelatesTo() const;
		void setRelatesTo(const QString& relatesTo);


};

} // namespace governikus
