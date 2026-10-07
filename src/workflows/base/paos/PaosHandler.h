/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "paos/PaosMessage.h"

#include <QSharedPointer>
#include <QXmlStreamReader>

namespace governikus
{

class PaosHandler
{
	Q_DISABLE_COPY(PaosHandler)

	private:
		QSharedPointer<PaosMessage> mParsedObject;

		void setParsedObject(std::unique_ptr<PaosMessage> pParsedObject);

	public:
		explicit PaosHandler(QIODevice* pDevice, bool pLoggingAllowed);

		[[nodiscard]] PaosType getDetectedPaosType() const;
		[[nodiscard]] const QSharedPointer<PaosMessage>& getPaosMessage() const;
};

} // namespace governikus
