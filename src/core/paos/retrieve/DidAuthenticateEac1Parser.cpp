/*!
 * DidAuthenticateEac1Parser.cpp
 *
 * \brief Parser for the PAOS DidAuthenticateEac1 element.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "asn1/ASN1Util.h"
#include "paos/element/ConnectionHandleParser.h"
#include "paos/invoke/PaosCreator.h"
#include "paos/retrieve/DidAuthenticateEac1Parser.h"


using namespace governikus;


DidAuthenticateEac1Parser::DidAuthenticateEac1Parser()
	: PaosParser(QStringLiteral("DIDAuthenticate"))
{
}


DidAuthenticateEac1Parser::~DidAuthenticateEac1Parser()
{
}


PaosMessage* DidAuthenticateEac1Parser::parseMessage()
{
	mDidAuthenticateEac1.reset(new DIDAuthenticateEAC1());

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
				mDidAuthenticateEac1->setConnectionHandle(ConnectionHandleParser(mXmlReader).parse());
			}
		}
		else if (mXmlReader->name() == QLatin1String("DIDName"))
		{
			if (readUniqueElementText(didName))
			{
				mDidAuthenticateEac1->setDidName(didName);
			}
		}
		else if (mXmlReader->name() == QLatin1String("AuthenticationProtocolData"))
		{
			QString ns = PaosCreator::getNamespace(PaosCreator::Namespace::XSI);
			QStringRef value = mXmlReader->attributes().value(ns, QStringLiteral("type"));
			if (value.endsWith(QLatin1String("EAC1InputType")))
			{
				mDidAuthenticateEac1->setEac1InputType(parseEac1InputType());
			}
		}
		else
		{
			qCWarning(paos) << "Unknown element:" << mXmlReader->name();
			mXmlReader->skipCurrentElement();
		}
	}

	return mParseError ? nullptr : mDidAuthenticateEac1.take();
}


Eac1InputType DidAuthenticateEac1Parser::parseEac1InputType()
{
	Eac1InputType eac1;

	QString certificateDescription, requiredCHAT, optionalCHAT, authenticatedAuxiliaryData, transactionInfo;
	while (readNextStartElement())
	{
		qCDebug(paos) << mXmlReader->name();
		if (mXmlReader->name() == QLatin1String("CertificateDescription"))
		{
			if (readUniqueElementText(certificateDescription))
			{
				const QByteArray certDesc = certificateDescription.toLatin1();
				eac1.setCertificateDescriptionAsBinary(QByteArray::fromHex(certDesc));
				eac1.setCertificateDescription(CertificateDescription::fromHex(certDesc));
				if (eac1.getCertificateDescription() == nullptr)
				{
					qCCritical(paos) << "Cannot parse CertificateDescription";
					mParseError = true;
				}
			}
		}
		else if (mXmlReader->name() == QLatin1String("RequiredCHAT"))
		{
			if (readUniqueElementText(requiredCHAT))
			{
				eac1.setRequiredChat(CHAT::fromHex(requiredCHAT.toLatin1()));
				if (eac1.getRequiredChat() == nullptr)
				{
					qCCritical(paos) << "Cannot parse required CHAT";
					mParseError = true;
				}
			}
		}
		else if (mXmlReader->name() == QLatin1String("OptionalCHAT"))
		{
			if (readUniqueElementText(optionalCHAT))
			{
				eac1.setOptionalChat(CHAT::fromHex(optionalCHAT.toLatin1()));
				if (eac1.getOptionalChat() == nullptr)
				{
					qCCritical(paos) << "Cannot parse optional CHAT";
					mParseError = true;
				}
			}
		}
		else if (mXmlReader->name() == QLatin1String("AuthenticatedAuxiliaryData"))
		{
			if (readUniqueElementText(authenticatedAuxiliaryData))
			{
				const QByteArray data = authenticatedAuxiliaryData.toUtf8();
				eac1.setAuthenticatedAuxiliaryDataAsBinary(QByteArray::fromHex(data));
				eac1.setAuthenticatedAuxiliaryData(AuthenticatedAuxiliaryData::fromHex(data));
				if (eac1.getAuthenticatedAuxiliaryData() == nullptr)
				{
					qCCritical(paos) << "Cannot parse AuthenticatedAuxiliaryData";
					mParseError = true;
				}
			}
		}
		else if (mXmlReader->name() == QLatin1String("TransactionInfo"))
		{
			if (readUniqueElementText(transactionInfo))
			{
				eac1.setTransactionInfo(transactionInfo);
			}
		}
		else if (mXmlReader->name() == QLatin1String("Certificate"))
		{
			if (auto cvc = CVCertificate::fromHex(readElementText().toLatin1()))
			{
				eac1.appendCvcerts(cvc);
			}
			else
			{
				qCCritical(paos) << "Cannot parse Certificate";
				mParseError = true;
			}
		}
		else
		{
			qCWarning(paos) << "Unknown element:" << mXmlReader->name();
			mXmlReader->skipCurrentElement();
		}
	}
	assertMandatoryList<QSharedPointer<CVCertificate> >(eac1.getCvCertificates(), "Certificate");

	return eac1;
}
