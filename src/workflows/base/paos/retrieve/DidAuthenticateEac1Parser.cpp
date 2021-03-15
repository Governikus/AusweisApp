/*!
 * \brief Parser for the PAOS DidAuthenticateEac1 element.
 *
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#include "paos/retrieve/DidAuthenticateEac1Parser.h"

#include "asn1/ASN1Util.h"
#include "paos/element/ConnectionHandleParser.h"
#include "paos/invoke/PaosCreator.h"

#include <QXmlStreamReader>

using namespace governikus;


DidAuthenticateEac1Parser::DidAuthenticateEac1Parser()
	: PaosParser(QStringLiteral("DIDAuthenticate"))
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
				ConnectionHandleParser parser(mXmlReader);
				mDidAuthenticateEac1->setConnectionHandle(parser.parse());
				mParseError |= parser.parserFailed();
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
			const auto value = mXmlReader->attributes().value(ns, QStringLiteral("type"));
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
			parseCertificateDescription(eac1, certificateDescription);
		}
		else if (mXmlReader->name() == QLatin1String("RequiredCHAT"))
		{
			parseRequiredCHAT(eac1, requiredCHAT);
		}
		else if (mXmlReader->name() == QLatin1String("OptionalCHAT"))
		{
			parseOptionalCHAT(eac1, optionalCHAT);
		}
		else if (mXmlReader->name() == QLatin1String("AuthenticatedAuxiliaryData"))
		{
			parseAuthenticatedAuxiliaryData(eac1, authenticatedAuxiliaryData);
		}
		else if (mXmlReader->name() == QLatin1String("TransactionInfo"))
		{
			parseTransactionInfo(eac1, transactionInfo);
		}
		else if (mXmlReader->name() == QLatin1String("Certificate"))
		{
			parseCertificate(eac1);
		}
		else
		{
			qCWarning(paos) << "Unknown element:" << mXmlReader->name();
			mXmlReader->skipCurrentElement();
		}
	}
	assertMandatoryList<QSharedPointer<const CVCertificate> >(eac1.getCvCertificates(), "Certificate");

	return eac1;
}


void DidAuthenticateEac1Parser::parseCertificateDescription(Eac1InputType& pEac1, QString& pCertificateDescription)
{
	if (readUniqueElementText(pCertificateDescription))
	{
		const QByteArray certDesc = pCertificateDescription.toLatin1();
		pEac1.setCertificateDescriptionAsBinary(QByteArray::fromHex(certDesc));
		pEac1.setCertificateDescription(CertificateDescription::fromHex(certDesc));
		if (pEac1.getCertificateDescription() == nullptr)
		{
			qCCritical(paos) << "Cannot parse CertificateDescription";
			mParseError = true;
		}
	}
}


void DidAuthenticateEac1Parser::parseRequiredCHAT(Eac1InputType& pEac1, QString& pRequiredCHAT)
{
	if (readUniqueElementText(pRequiredCHAT))
	{
		pEac1.setRequiredChat(CHAT::fromHex(pRequiredCHAT.toLatin1()));
		if (pEac1.getRequiredChat() == nullptr)
		{
			qCCritical(paos) << "Cannot parse required CHAT";
			mParseError = true;
		}
		else
		{
			qCDebug(paos) << "Access rights:" << pEac1.getRequiredChat()->getAccessRights();
		}
	}
}


void DidAuthenticateEac1Parser::parseOptionalCHAT(Eac1InputType& pEac1, QString& pOptionalCHAT)
{
	if (readUniqueElementText(pOptionalCHAT))
	{
		pEac1.setOptionalChat(CHAT::fromHex(pOptionalCHAT.toLatin1()));
		if (pEac1.getOptionalChat() == nullptr)
		{
			qCCritical(paos) << "Cannot parse optional CHAT";
			mParseError = true;
		}
		else
		{
			qCDebug(paos) << "Access rights:" << pEac1.getOptionalChat()->getAccessRights();
		}
	}
}


void DidAuthenticateEac1Parser::parseAuthenticatedAuxiliaryData(Eac1InputType& pEac1, QString& pAuthenticatedAuxiliaryData)
{
	if (readUniqueElementText(pAuthenticatedAuxiliaryData))
	{
		const QByteArray data = pAuthenticatedAuxiliaryData.toUtf8();
		pEac1.setAuthenticatedAuxiliaryDataAsBinary(QByteArray::fromHex(data));
		pEac1.setAuthenticatedAuxiliaryData(AuthenticatedAuxiliaryData::fromHex(data));
		if (pEac1.getAuthenticatedAuxiliaryData() == nullptr)
		{
			qCCritical(paos) << "Cannot parse AuthenticatedAuxiliaryData";
			mParseError = true;
		}
	}
}


void DidAuthenticateEac1Parser::parseTransactionInfo(Eac1InputType& pEac1, QString& pTransactionInfo)
{
	if (readUniqueElementText(pTransactionInfo))
	{
		pEac1.setTransactionInfo(pTransactionInfo);
	}
}


void DidAuthenticateEac1Parser::parseCertificate(Eac1InputType& pEac1)
{
	if (auto cvc = CVCertificate::fromHex(readElementText().toLatin1()))
	{
		qCDebug(paos) << "Linked Certificate (Authority):" << cvc->getBody().getCertificationAuthorityReference();
		qCDebug(paos) << "Certificate Name (Holder):" << cvc->getBody().getCertificateHolderReference();
		pEac1.appendCvcerts(cvc);
	}
	else
	{
		qCCritical(paos) << "Cannot parse Certificate";
		mParseError = true;
	}
}
