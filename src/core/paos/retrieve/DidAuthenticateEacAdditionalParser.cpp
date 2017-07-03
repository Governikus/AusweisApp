/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "paos/retrieve/DidAuthenticateEacAdditionalParser.h"

#include "paos/element/ConnectionHandleParser.h"
#include "paos/invoke/PaosCreator.h"
#include "paos/retrieve/DidAuthenticateEacAdditional.h"

#include <QDebug>

using namespace governikus;


DidAuthenticateEacAdditionalParser::DidAuthenticateEacAdditionalParser()
	: PaosParser(QStringLiteral("DIDAuthenticate"))
{

}


DidAuthenticateEacAdditionalParser::~DidAuthenticateEacAdditionalParser()
{
}


PaosMessage* DidAuthenticateEacAdditionalParser::parseMessage()
{
	mDidAuthenticateEacAdditional.reset(new DIDAuthenticateEACAdditional());

	bool isConnectionHandleNotSet = true;
	QString didName;

	while (readNextStartElement())
	{
		qCDebug(paos) << mXmlReader->name();
		if (mXmlReader->name() == QLatin1String("ConnectionHandle"))
		{
			if (assertNoDuplicateElement(isConnectionHandleNotSet))
			{
				isConnectionHandleNotSet = false;
				mDidAuthenticateEacAdditional->setConnectionHandle(ConnectionHandleParser(mXmlReader).parse());
			}
		}
		else if (mXmlReader->name() == QLatin1String("DIDName"))
		{
			if (readUniqueElementText(didName))
			{
				mDidAuthenticateEacAdditional->setDidName(didName);
			}
		}
		else if (mXmlReader->name() == QLatin1String("AuthenticationProtocolData"))
		{
			QString ns = PaosCreator::getNamespace(PaosCreator::Namespace::XSI);
			QStringRef value = mXmlReader->attributes().value(ns, QStringLiteral("type"));
			if (value.endsWith(QLatin1String("EACAdditionalInputType")))
			{
				mDidAuthenticateEacAdditional->setSignature(parseEacAdditionalInputType());
			}
		}
		else
		{
			qCWarning(paos) << "Unknown element:" << mXmlReader->name();
			mXmlReader->skipCurrentElement();
		}
	}

	return mParseError ? nullptr : mDidAuthenticateEacAdditional.take();
}


QString DidAuthenticateEacAdditionalParser::parseEacAdditionalInputType()
{

	QString signature;
	while (readNextStartElement())
	{
		qCDebug(paos) << mXmlReader->name();
		if (mXmlReader->name() == QLatin1String("Signature"))
		{
			readUniqueElementText(signature);
		}
		else
		{
			qCWarning(paos) << "Unknown element:" << mXmlReader->name();
			mXmlReader->skipCurrentElement();
		}
	}

	assertMandatoryElement(signature, "Signature");
	return signature;
}
