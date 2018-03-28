/*!
 * \brief Generic Handler to detect and parse paos types.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
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

		virtual bool handleFoundElement(const QString& pElementName, const QString& pValue, const QXmlStreamAttributes& pAttributes) override;

	public:
		PaosHandler(const QByteArray& pXmlData);

		PaosType getDetectedPaosType() const;
		QSharedPointer<PaosMessage> getPaosMessage() const;
};

} /* namespace governikus */
