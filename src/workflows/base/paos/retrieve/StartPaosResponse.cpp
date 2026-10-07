/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#include "StartPaosResponse.h"


using namespace governikus;


void StartPaosResponse::parse()
{
	while (mParser->readNextStartElement())
	{
		const auto& name = mParser->getElementName();
		if (name == QLatin1String("Result"))
		{
			parseResult();
		}
		else
		{
			mParser->skipCurrentElement();
		}
	}
}


void StartPaosResponse::parseResult()
{
	while (mParser->readNextStartElement())
	{
		const auto& name = mParser->getElementName();
		if (name == QLatin1String("ResultMajor"))
		{
			mResultMajor = mParser->readElementText();
		}
		else if (name == QLatin1String("ResultMinor"))
		{
			mResultMinor = mParser->readElementText();
		}
		else if (name == QLatin1String("ResultMessage"))
		{
			mResultMessage = mParser->readElementText();
		}
		else
		{
			mParser->skipCurrentElement();
		}
	}
}


StartPaosResponse::StartPaosResponse(const QSharedPointer<ElementParser>& pParser)
	: ResponseType(PaosType::STARTPAOS_RESPONSE)
	, mParser(pParser)
	, mResultMajor()
	, mResultMinor()
	, mResultMessage()
{
	parse();
	setResult(ECardApiResult(mResultMajor, mResultMinor, mResultMessage, ECardApiResult::Origin::Server));
}


StartPaosResponse::~StartPaosResponse() = default;
