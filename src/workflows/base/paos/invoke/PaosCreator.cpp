/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "PaosCreator.h"

#include <QDebug>
#include <QUuid>

using namespace governikus;


const QMap<PaosCreator::Namespace, QString> PaosCreator::mNamespacePrefix = {
	{PaosCreator::Namespace::DEFAULT, QString()},
	{PaosCreator::Namespace::ADDRESSING, QStringLiteral("wsa")},
	{PaosCreator::Namespace::DSS, QStringLiteral("dss")},
	{PaosCreator::Namespace::ECARD, QStringLiteral("ecard")},
	{PaosCreator::Namespace::PAOS, QStringLiteral("paos")},
	{PaosCreator::Namespace::TECHSCHEMA, QStringLiteral("iso")},
	{PaosCreator::Namespace::XSD, QStringLiteral("xsd")},
	{PaosCreator::Namespace::XSI, QStringLiteral("xsi")},
	{PaosCreator::Namespace::SOAP, QStringLiteral("soap")}
};

const QMap<PaosCreator::Namespace, QString> PaosCreator::mNamespace = {
	{PaosCreator::Namespace::ADDRESSING, QStringLiteral("http://www.w3.org/2005/03/addressing")},
	{PaosCreator::Namespace::DSS, QStringLiteral("urn:oasis:names:tc:dss:1.0:core:schema")},
	{PaosCreator::Namespace::ECARD, QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1")},
	{PaosCreator::Namespace::PAOS, QStringLiteral("urn:liberty:paos:2006-08")},
	{PaosCreator::Namespace::TECHSCHEMA, QStringLiteral("urn:iso:std:iso-iec:24727:tech:schema")},
	{PaosCreator::Namespace::XSD, QStringLiteral("http://www.w3.org/2001/XMLSchema")},
	{PaosCreator::Namespace::XSI, QStringLiteral("http://www.w3.org/2001/XMLSchema-instance")},
	{PaosCreator::Namespace::SOAP, QStringLiteral("http://schemas.xmlsoap.org/soap/envelope/")}
};

PaosCreator::PaosCreator()
	: mContent()
	, mRelatedMessageId()
	, mWriter(&mContent)
{
	mWriter.setAutoFormatting(true);
}


PaosCreator::~PaosCreator()
{
}


QString PaosCreator::getNamespace(Namespace prefix)
{
	return mNamespace.value(prefix);
}


QString PaosCreator::getNamespaceType(Namespace pPrefix, const QString& pType)
{
	QString prefix = mNamespacePrefix.value(pPrefix);
	Q_ASSERT(!prefix.isEmpty());
	return prefix + QLatin1Char(':') + pType;
}


QString PaosCreator::getNamespacePrefix(Namespace pPrefix, const QString& pSuffix)
{
	QString value = mNamespacePrefix.value(pPrefix);
	if (pSuffix.isNull() || value.isEmpty())
	{
		Q_ASSERT(pSuffix.isNull());
		if (!value.isEmpty())
		{
			value.prepend(QLatin1Char(':'));
		}
		value.prepend(QLatin1String("xmlns"));
	}
	else
	{
		Q_ASSERT(pPrefix != Namespace::DEFAULT);
		Q_ASSERT(!value.isEmpty());
		value += QLatin1Char(':');
		value += pSuffix;
	}
	return value;
}


QByteArray PaosCreator::marshall()
{
	if (mContent.isNull())
	{
		createEnvelopeElement();
	}
	return mContent;
}


void PaosCreator::setRelatedMessageId(const QString& pId)
{
	mRelatedMessageId = pId;
}


void PaosCreator::createHeaderElement()
{
	mWriter.writeStartElement(getNamespacePrefix(Namespace::SOAP, QStringLiteral("Header")));

	mWriter.writeStartElement(getNamespacePrefix(Namespace::PAOS, QStringLiteral("PAOS")));
	{
		mWriter.writeAttribute(getNamespacePrefix(Namespace::SOAP, QStringLiteral("mustUnderstand")), QStringLiteral("1"));
		mWriter.writeAttribute(getNamespacePrefix(Namespace::SOAP, QStringLiteral("actor")), QStringLiteral("http://schemas.xmlsoap.org/soap/actor/next"));
		mWriter.writeTextElement(getNamespaceType(Namespace::PAOS, QStringLiteral("Version")), getNamespace(Namespace::PAOS));

		mWriter.writeStartElement(getNamespaceType(Namespace::PAOS, QStringLiteral("EndpointReference")));
		{
			mWriter.writeTextElement(getNamespaceType(Namespace::PAOS, QStringLiteral("Address")), QStringLiteral("http://www.projectliberty.org/2006/01/role/paos"));

			mWriter.writeStartElement(getNamespaceType(Namespace::PAOS, QStringLiteral("MetaData")));
			{
				mWriter.writeTextElement(getNamespaceType(Namespace::PAOS, QStringLiteral("ServiceType")), QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/PAOS/GetNextCommand"));
			}
			mWriter.writeEndElement(); // MetaData
		}
		mWriter.writeEndElement(); // EndpointReference
	}
	mWriter.writeEndElement(); // PAOS

	mWriter.writeStartElement(getNamespaceType(Namespace::ADDRESSING, QStringLiteral("ReplyTo")));
	{
		mWriter.writeTextElement(getNamespaceType(Namespace::ADDRESSING, QStringLiteral("Address")), QStringLiteral("http://www.projectliberty.org/2006/02/role/paos"));
	}
	mWriter.writeEndElement(); // ReplyTo

	if (!mRelatedMessageId.isNull())
	{
		mWriter.writeTextElement(getNamespaceType(Namespace::ADDRESSING, QStringLiteral("RelatesTo")), mRelatedMessageId);
	}

	const auto& id = QStringLiteral("urn:uuid:") + QUuid::createUuid().toString(QUuid::WithoutBraces);
	mWriter.writeTextElement(getNamespaceType(Namespace::ADDRESSING, QStringLiteral("MessageID")), id);

	mWriter.writeEndElement(); // Header
}


void PaosCreator::writeTextElement(const QString& pQualifiedName, const QByteArray& pText)
{
	mWriter.writeTextElement(pQualifiedName, QString::fromLatin1(pText));
}


void PaosCreator::createEnvelopeElement()
{
	mWriter.writeStartDocument();

	mWriter.writeStartElement(getNamespacePrefix(Namespace::SOAP, QStringLiteral("Envelope")));
	mWriter.writeAttribute(getNamespacePrefix(Namespace::SOAP), getNamespace(Namespace::SOAP));
	mWriter.writeAttribute(getNamespacePrefix(Namespace::XSD), getNamespace(Namespace::XSD));
	mWriter.writeAttribute(getNamespacePrefix(Namespace::XSI), getNamespace(Namespace::XSI));
	mWriter.writeAttribute(getNamespacePrefix(Namespace::PAOS), getNamespace(Namespace::PAOS));
	mWriter.writeAttribute(getNamespacePrefix(Namespace::ADDRESSING), getNamespace(Namespace::ADDRESSING));
	mWriter.writeAttribute(getNamespacePrefix(Namespace::DSS), getNamespace(Namespace::DSS));
	mWriter.writeAttribute(getNamespacePrefix(Namespace::ECARD), getNamespace(Namespace::ECARD));
	mWriter.writeAttribute(getNamespacePrefix(Namespace::TECHSCHEMA), getNamespace(Namespace::TECHSCHEMA));

	createHeaderElement();

	mWriter.writeStartElement(getNamespacePrefix(Namespace::SOAP, QStringLiteral("Body")));
	createBodyElement();
	mWriter.writeEndElement(); // Body

	mWriter.writeEndElement(); // Envelope

	mWriter.writeEndDocument();
}


void PaosCreator::createResultElement(const ResponseType& pResponse)
{
	mWriter.writeStartElement(QStringLiteral("Result"));
	mWriter.writeAttribute(getNamespacePrefix(Namespace::DEFAULT), getNamespace(Namespace::DSS));

	const ECardApiResult& result = pResponse.getResult();
	mWriter.writeTextElement(QStringLiteral("ResultMajor"), result.getMajorString());
	if (result.getMinor() != ECardApiResult::Minor::null)
	{
		mWriter.writeTextElement(QStringLiteral("ResultMinor"), result.getMinorString());
	}

	if (!result.getMessage().isNull())
	{
		mWriter.writeStartElement(QStringLiteral("ResultMessage"));
		mWriter.writeAttribute(QStringLiteral("xml:lang"), result.getMessageLang());
		mWriter.writeCharacters(result.getMessage());
		mWriter.writeEndElement(); // ResultMessage
	}

	mWriter.writeEndElement(); // Result
}
