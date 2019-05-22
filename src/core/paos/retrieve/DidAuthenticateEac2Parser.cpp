/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "paos/retrieve/DidAuthenticateEac2Parser.h"

#include "paos/element/ConnectionHandleParser.h"
#include "paos/invoke/PaosCreator.h"
#include "paos/retrieve/DidAuthenticateEac2.h"

#include <QDebug>


using namespace governikus;


DidAuthenticateEac2Parser::DidAuthenticateEac2Parser()
	: PaosParser(QStringLiteral("DIDAuthenticate"))
{
}


DidAuthenticateEac2Parser::~DidAuthenticateEac2Parser()
{
}


PaosMessage* DidAuthenticateEac2Parser::parseMessage()
{
	mDidAuthenticateEac2.reset(new DIDAuthenticateEAC2());

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
				mDidAuthenticateEac2->setConnectionHandle(ConnectionHandleParser(mXmlReader).parse());
			}
		}
		else if (mXmlReader->name() == QLatin1String("DIDName"))
		{
			if (readUniqueElementText(didName))
			{
				mDidAuthenticateEac2->setDidName(didName);
			}
		}
		else if (mXmlReader->name() == QLatin1String("AuthenticationProtocolData"))
		{
			QString ns = PaosCreator::getNamespace(PaosCreator::Namespace::XSI);
			QStringRef value = mXmlReader->attributes().value(ns, QStringLiteral("type"));
			if (value.endsWith(QStringLiteral("EAC2InputType")))
			{
				mDidAuthenticateEac2->setEac2InputType(parseEac2InputType());
			}
		}
		else
		{
			qCWarning(paos) << "Unknown element:" << mXmlReader->name();
			mXmlReader->skipCurrentElement();
		}
	}

	return mParseError ? nullptr : mDidAuthenticateEac2.take();
}


Eac2InputType DidAuthenticateEac2Parser::parseEac2InputType()
{
	Eac2InputType eac2;

	QString ephemeralPublicKey, signature;
	while (readNextStartElement())
	{
		qCDebug(paos) << mXmlReader->name();
		if (mXmlReader->name() == QLatin1String("Certificate"))
		{
			const QByteArray hexCvc = readElementText().toLatin1();
			if (auto cvc = CVCertificate::fromHex(hexCvc))
			{
				eac2.appendCvcert(cvc);
				eac2.appendCvcertAsBinary(QByteArray::fromHex(hexCvc));
			}
			else
			{
				qCCritical(paos) << "Cannot parse Certificate";
				mParseError = true;
			}
		}
		else if (mXmlReader->name() == QLatin1String("EphemeralPublicKey"))
		{
			if (readUniqueElementText(ephemeralPublicKey))
			{
				eac2.setEphemeralPublicKey(ephemeralPublicKey);
			}
		}
		else if (mXmlReader->name() == QLatin1String("Signature"))
		{
			if (readUniqueElementText(signature))
			{
				eac2.setSignature(signature);
			}
		}
		else
		{
			qCWarning(paos) << "Unknown element:" << mXmlReader->name();
			mXmlReader->skipCurrentElement();
		}
	}

	assertMandatoryElement(eac2.getEphemeralPublicKey(), "EphemeralPublicKey");

	return eac2;
}
