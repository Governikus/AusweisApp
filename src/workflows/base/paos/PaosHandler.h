/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "paos/ElementDetector.h"
#include "paos/PaosMessage.h"

#include <QSharedPointer>
#include <QXmlStreamReader>

namespace governikus
{

class PaosHandler
	: private ElementDetector
{
	Q_DISABLE_COPY(PaosHandler)

	private:
		const QByteArray mXmlData;
		PaosType mDetectedType;
		QSharedPointer<PaosMessage> mParsedObject;

		void detect();
		void parse();
		void setParsedObject(PaosMessage* pParsedObject);

		bool handleFoundElement(QStringView pElementName, const QString& pValue, const QXmlStreamAttributes& pAttributes) override;

	public:
		explicit PaosHandler(const QByteArray& pXmlData);

		[[nodiscard]] PaosType getDetectedPaosType() const;
		[[nodiscard]] QSharedPointer<PaosMessage> getPaosMessage() const;
};

} // namespace governikus
