/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "PaosHandler.h"

#include "paos/retrieve/InitializeFramework.h"
#include "paos/retrieve/StartPaosResponse.h"
#include "retrieve/DidAuthenticateEac1Parser.h"
#include "retrieve/DidAuthenticateEac2Parser.h"
#include "retrieve/DidAuthenticateEacAdditionalParser.h"
#include "retrieve/TransmitParser.h"


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(paos)


PaosHandler::PaosHandler(const QByteArray& pXmlData)
	: ElementDetector(pXmlData)
	, mXmlData(pXmlData)
	, mDetectedType(PaosType::UNKNOWN)
	, mParsedObject()
{
	detect();
	parse();
}


void PaosHandler::parse()
{
	if (mDetectedType == PaosType::INITIALIZE_FRAMEWORK)
	{
		setParsedObject(new InitializeFramework(mXmlData));
	}
	else if (mDetectedType == PaosType::DID_AUTHENTICATE_EAC1)
	{
		setParsedObject(DidAuthenticateEac1Parser().parse(mXmlData));
	}
	else if (mDetectedType == PaosType::DID_AUTHENTICATE_EAC2)
	{
		setParsedObject(DidAuthenticateEac2Parser().parse(mXmlData));
	}
	else if (mDetectedType == PaosType::DID_AUTHENTICATE_EAC_ADDITIONAL_INPUT_TYPE)
	{
		setParsedObject(DidAuthenticateEacAdditionalParser().parse(mXmlData));
	}
	else if (mDetectedType == PaosType::TRANSMIT)
	{
		setParsedObject(TransmitParser().parse(mXmlData));
	}
	else if (mDetectedType == PaosType::STARTPAOS_RESPONSE)
	{
		setParsedObject(new StartPaosResponse(mXmlData));
	}
}


void PaosHandler::setParsedObject(PaosMessage* pParsedObject)
{
	if (pParsedObject == nullptr)
	{
		qCCritical(paos) << "Error parsing message. This is not a valid" << mDetectedType;
		mDetectedType = PaosType::UNKNOWN;
	}
	else
	{
		mParsedObject = QSharedPointer<PaosMessage>(pParsedObject);
	}
}


bool PaosHandler::handleFoundElement(QStringView pElementName, const QString&, const QXmlStreamAttributes& pAttributes)
{
	if (pElementName == QLatin1String("InitializeFramework"))
	{
		mDetectedType = PaosType::INITIALIZE_FRAMEWORK;
	}
	else if (pElementName == QLatin1String("DIDAuthenticate"))
	{
		detectStartElements({QStringLiteral("AuthenticationProtocolData")});
	}
	else if (pElementName == QLatin1String("AuthenticationProtocolData"))
	{
		for (const auto& attribute : pAttributes)
		{
			if (attribute.value().endsWith(QLatin1String("EAC1InputType")))
			{
				mDetectedType = PaosType::DID_AUTHENTICATE_EAC1;
			}
			else if (attribute.value().endsWith(QLatin1String("EAC2InputType")))
			{
				mDetectedType = PaosType::DID_AUTHENTICATE_EAC2;
			}
			else if (attribute.value().endsWith(QLatin1String("EACAdditionalInputType")))
			{
				mDetectedType = PaosType::DID_AUTHENTICATE_EAC_ADDITIONAL_INPUT_TYPE;
			}
		}
	}
	else if (pElementName == QLatin1String("Transmit"))
	{
		mDetectedType = PaosType::TRANSMIT;
	}
	else if (pElementName == QLatin1String("StartPAOSResponse"))
	{
		mDetectedType = PaosType::STARTPAOS_RESPONSE;
	}

	return mDetectedType == PaosType::UNKNOWN;
}


void PaosHandler::detect()
{
	const QStringList expectedElements({
				QStringLiteral("InitializeFramework"),
				QStringLiteral("DIDList"),
				QStringLiteral("DIDAuthenticate"),
				QStringLiteral("Transmit"),
				QStringLiteral("Disconnect"),
				QStringLiteral("StartPAOSResponse")
			});

	detectStartElements(expectedElements);
}


PaosType PaosHandler::getDetectedPaosType() const
{
	return mDetectedType;
}


QSharedPointer<PaosMessage> PaosHandler::getPaosMessage() const
{
	return mParsedObject;
}
