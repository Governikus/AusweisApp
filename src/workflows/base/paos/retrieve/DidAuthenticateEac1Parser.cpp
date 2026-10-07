/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#include "paos/retrieve/DidAuthenticateEac1Parser.h"


#include <QLoggingCategory>
#include <QRegularExpression>
#include <QXmlStreamReader>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(paos)


Eac1InputType DidAuthenticateEac1Parser::parseEac1InputType()
{
	Eac1InputType eac1;

	QString certificateDescription;
	QString requiredCHAT;
	QString optionalCHAT;
	QString authenticatedAuxiliaryData;
	QString transactionInfo;

	while (mParser->readNextStartElement())
	{
		const auto& name = mParser->getElementName();
		if (name == QLatin1String("CertificateDescription"))
		{
			parseCertificateDescription(eac1, certificateDescription);
		}
		else if (name == QLatin1String("RequiredCHAT"))
		{
			parseRequiredCHAT(eac1, requiredCHAT);
		}
		else if (name == QLatin1String("OptionalCHAT"))
		{
			parseOptionalCHAT(eac1, optionalCHAT);
		}
		else if (name == QLatin1String("AuthenticatedAuxiliaryData"))
		{
			parseAuthenticatedAuxiliaryData(eac1, authenticatedAuxiliaryData);
		}
		else if (name == QLatin1String("TransactionInfo"))
		{
			parseTransactionInfo(eac1, transactionInfo);
		}
		else if (name == QLatin1String("Certificate"))
		{
			parseCertificate(eac1);
		}
		else if (name == QLatin1String("AcceptedEIDType"))
		{
			parseAcceptedEidType(eac1);
		}
		else
		{
			qCWarning(paos) << "Unknown element:" << name;
			mParser->skipCurrentElement();
		}
	}
	mParser->assertMandatoryList<QSharedPointer<const CVCertificate>>(eac1.getCvCertificates(), "Certificate");
	if (eac1.getAcceptedEidTypes().isEmpty()) // For legacy eID-Server without explicit Smart-eID information
	{
		eac1.appendAcceptedEidType(AcceptedEidType::CARD_CERTIFIED);
		eac1.appendAcceptedEidType(AcceptedEidType::SE_CERTIFIED);
		eac1.appendAcceptedEidType(AcceptedEidType::SE_ENDORSED);
	}

	return eac1;
}


void DidAuthenticateEac1Parser::parseCertificateDescription(Eac1InputType& pEac1, QString& pCertificateDescription)
{

	if (mParser->readUniqueElementText(pCertificateDescription))
	{
		const QByteArray certDesc = pCertificateDescription.toLatin1();
		pEac1.setCertificateDescriptionAsBinary(QByteArray::fromHex(certDesc));
		pEac1.setCertificateDescription(CertificateDescription::fromHex(certDesc));
		if (pEac1.getCertificateDescription() == nullptr)
		{
			qCCritical(paos) << "Cannot parse CertificateDescription";
			mParser->setParserFailed();
		}
	}
}


void DidAuthenticateEac1Parser::parseRequiredCHAT(Eac1InputType& pEac1, QString& pRequiredCHAT)
{
	if (mParser->readUniqueElementText(pRequiredCHAT))
	{
		pEac1.setRequiredChat(CHAT::fromHex(pRequiredCHAT.toLatin1()));
		if (pEac1.getRequiredChat() == nullptr)
		{
			qCCritical(paos) << "Cannot parse required CHAT";
			mParser->setParserFailed();
		}
		else
		{
			qCDebug(paos) << "Access rights:" << pEac1.getRequiredChat()->getAccessRights();
		}
	}
}


void DidAuthenticateEac1Parser::parseOptionalCHAT(Eac1InputType& pEac1, QString& pOptionalCHAT)
{
	if (mParser->readUniqueElementText(pOptionalCHAT))
	{
		pEac1.setOptionalChat(CHAT::fromHex(pOptionalCHAT.toLatin1()));
		if (pEac1.getOptionalChat() == nullptr)
		{
			qCCritical(paos) << "Cannot parse optional CHAT";
			mParser->setParserFailed();
		}
		else
		{
			qCDebug(paos) << "Access rights:" << pEac1.getOptionalChat()->getAccessRights();
		}
	}
}


void DidAuthenticateEac1Parser::parseAuthenticatedAuxiliaryData(Eac1InputType& pEac1, QString& pAuthenticatedAuxiliaryData)
{
	if (mParser->readUniqueElementText(pAuthenticatedAuxiliaryData))
	{
		const QByteArray data = pAuthenticatedAuxiliaryData.toUtf8();
		pEac1.setAuthenticatedAuxiliaryDataAsBinary(QByteArray::fromHex(data));
		pEac1.setAuthenticatedAuxiliaryData(AuthenticatedAuxiliaryData::fromHex(data));
		if (pEac1.getAuthenticatedAuxiliaryData() == nullptr)
		{
			qCCritical(paos) << "Cannot parse AuthenticatedAuxiliaryData";
			mParser->setParserFailed();
		}
	}
}


void DidAuthenticateEac1Parser::parseTransactionInfo(Eac1InputType& pEac1, QString& pTransactionInfo)
{
	if (mParser->readUniqueElementText(pTransactionInfo))
	{
		pEac1.setTransactionInfo(pTransactionInfo);
	}
}


void DidAuthenticateEac1Parser::parseCertificate(Eac1InputType& pEac1)
{
	if (auto cvc = CVCertificate::fromRaw(QByteArray::fromHex(mParser->readElementText().toLatin1())))
	{
		pEac1.appendCvcerts(cvc);
	}
	else
	{
		qCCritical(paos) << "Cannot parse Certificate";
		mParser->setParserFailed();
	}
}


void DidAuthenticateEac1Parser::parseAcceptedEidType(Eac1InputType& pEac1)
{

	const auto& acceptedEidType = mParser->readElementText();
	qCDebug(paos) << "AcceptedEIDType:" << acceptedEidType;

	if (acceptedEidType == QLatin1String("CardCertified"))
	{
		pEac1.appendAcceptedEidType(AcceptedEidType::CARD_CERTIFIED);
	}
	else if (acceptedEidType == QLatin1String("SECertified"))
	{
		pEac1.appendAcceptedEidType(AcceptedEidType::SE_CERTIFIED);
	}
	else if (acceptedEidType == QLatin1String("SEEndorsed"))
	{
		pEac1.appendAcceptedEidType(AcceptedEidType::SE_ENDORSED);
	}
	else if (acceptedEidType == QLatin1String("HWKeyStore"))
	{
		pEac1.appendAcceptedEidType(AcceptedEidType::HW_KEYSTORE);
	}
	else
	{
		qCCritical(paos) << "Cannot parse AcceptedEidType";
		mParser->setParserFailed();
	}
}


DidAuthenticateEac1Parser::DidAuthenticateEac1Parser(const QSharedPointer<ElementParser>& pParser)
	: mParser(pParser)
{
}


std::unique_ptr<DIDAuthenticateEAC1> DidAuthenticateEac1Parser::parse()
{
	auto didAuthenticateEac1 = std::make_unique<DIDAuthenticateEAC1>();
	didAuthenticateEac1->setEac1InputType(parseEac1InputType());
	return mParser->parserFailed() ? nullptr : std::move(didAuthenticateEac1);
}
