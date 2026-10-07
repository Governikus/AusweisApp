/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#include "PaosHandler.h"

#include "paos/retrieve/PaosParser.h"


using namespace governikus;
using namespace std::placeholders;


Q_DECLARE_LOGGING_CATEGORY(paos)


void PaosHandler::setParsedObject(std::unique_ptr<PaosMessage> pParsedObject)
{
	if (pParsedObject == nullptr)
	{
		qCCritical(paos) << "Error parsing message.";
	}
	else
	{
		mParsedObject = QSharedPointer<PaosMessage>(pParsedObject.release());
	}
}


PaosHandler::PaosHandler(QIODevice* pDevice, bool pLoggingAllowed)
	: mParsedObject()
{
	const auto& parser = QSharedPointer<ElementParser>::create(QSharedPointer<QXmlStreamReader>::create(pDevice), pLoggingAllowed);
	setParsedObject(PaosParser().parse(parser));
}


PaosType PaosHandler::getDetectedPaosType() const
{
	return mParsedObject ? mParsedObject->mType : PaosType::UNKNOWN;
}


const QSharedPointer<PaosMessage>& PaosHandler::getPaosMessage() const
{
	return mParsedObject;
}
