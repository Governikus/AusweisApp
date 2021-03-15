/*!
 * \brief Generic Handler to detect and parse paos types.
 *
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
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
	private:
		PaosType mDetectedType;
		QSharedPointer<PaosMessage> mParsedObject;

		Q_DISABLE_COPY(PaosHandler)
		void detect();
		void parse();
		void setParsedObject(PaosMessage* pParsedObject);

		bool handleFoundElement(const QString& pElementName, const QString& pValue, const QXmlStreamAttributes& pAttributes) override;

	public:
		explicit PaosHandler(const QByteArray& pXmlData);

		[[nodiscard]] PaosType getDetectedPaosType() const;
		[[nodiscard]] QSharedPointer<PaosMessage> getPaosMessage() const;
};

} // namespace governikus
