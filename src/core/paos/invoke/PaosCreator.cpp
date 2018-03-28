/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
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
	: mDoc()
{

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
	if (mDoc.isNull())
	{
		mDoc.appendChild(getDocumentStructure());
	}
	return mDoc.toByteArray();
}


QDomElement PaosCreator::createTextElement(const QString& pName, const QByteArray& pContent)
{
	return createTextElement(pName, QString::fromLatin1(pContent));
}


QDomElement PaosCreator::createTextElement(const QString& pName, const QString& pContent)
{
	QDomElement element = mDoc.createElement(pName);
	element.appendChild(mDoc.createTextNode(pContent));
	return element;
}


QDomElement PaosCreator::createTextElement(PaosCreator::Namespace pNamespace, const QString& pName, const QByteArray& pContent)
{
	return createTextElement(pNamespace, pName, QString::fromLatin1(pContent));
}


QDomElement PaosCreator::createTextElement(PaosCreator::Namespace pNamespace, const QString& pName, const QString& pContent)
{
	QDomElement element = mDoc.createElement(getNamespacePrefix(pNamespace, pName));
	element.appendChild(mDoc.createTextNode(pContent));
	return element;
}


QDomElement PaosCreator::createHeaderElement(const QString& pRelatesTo, const QString& pMessageID)
{
	Q_ASSERT(!pMessageID.isEmpty());

	QDomElement headerElement = mDoc.createElement(getNamespacePrefix(Namespace::SOAP, QStringLiteral("Header")));

	QDomElement paosElement = mDoc.createElement(getNamespacePrefix(Namespace::PAOS, QStringLiteral("PAOS")));
	headerElement.appendChild(paosElement);
	paosElement.setAttribute(getNamespacePrefix(Namespace::SOAP, QStringLiteral("actor")), QStringLiteral("http://schemas.xmlsoap.org/soap/actor/next"));
	paosElement.setAttribute(getNamespacePrefix(Namespace::SOAP, QStringLiteral("mustUnderstand")), QStringLiteral("1"));
	QDomElement versionElement = createTextElement(getNamespaceType(Namespace::PAOS, QStringLiteral("Version")), getNamespace(Namespace::PAOS));
	paosElement.appendChild(versionElement);
	QDomElement endpointReferenceElement = mDoc.createElement(getNamespaceType(Namespace::PAOS, QStringLiteral("EndpointReference")));
	paosElement.appendChild(endpointReferenceElement);
	QDomElement addressElement = createTextElement(getNamespaceType(Namespace::PAOS, QStringLiteral("Address")), QStringLiteral("http://www.projectliberty.org/2006/01/role/paos"));
	endpointReferenceElement.appendChild(addressElement);
	QDomElement metaDataElement = mDoc.createElement(getNamespaceType(Namespace::PAOS, QStringLiteral("MetaData")));
	endpointReferenceElement.appendChild(metaDataElement);
	QDomElement serviceTypeElement = createTextElement(getNamespaceType(Namespace::PAOS, QStringLiteral("ServiceType")), QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/PAOS/GetNextCommand"));
	metaDataElement.appendChild(serviceTypeElement);

	QDomElement replyToElement = mDoc.createElement(getNamespaceType(Namespace::ADDRESSING, QStringLiteral("ReplyTo")));
	headerElement.appendChild(replyToElement);
	replyToElement.appendChild(createTextElement(getNamespaceType(Namespace::ADDRESSING, QStringLiteral("Address")), QStringLiteral("http://www.projectliberty.org/2006/02/role/paos")));

	if (!pRelatesTo.isNull())
	{
		QDomElement relatesToElement = createTextElement(getNamespaceType(Namespace::ADDRESSING, QStringLiteral("RelatesTo")), pRelatesTo);
		headerElement.appendChild(relatesToElement);
	}

	QDomElement messageIdElement = createTextElement(getNamespaceType(Namespace::ADDRESSING, QStringLiteral("MessageID")), pMessageID);
	headerElement.appendChild(messageIdElement);

	return headerElement;
}


QDomElement PaosCreator::createBodyElement(const QDomElement& pBody)
{
	QDomElement element = mDoc.createElement(getNamespacePrefix(Namespace::SOAP, QStringLiteral("Body")));
	element.appendChild(pBody);
	return element;
}


QDomElement PaosCreator::createEnvelopeElement(const QDomElement& pBody, const QString& pRelatesTo, const QString& pMessageID)
{
	QDomElement element = mDoc.createElement(getNamespacePrefix(Namespace::SOAP, QStringLiteral("Envelope")));
	element.setAttribute(getNamespacePrefix(Namespace::SOAP), getNamespace(Namespace::SOAP));
	element.setAttribute(getNamespacePrefix(Namespace::XSD), getNamespace(Namespace::XSD));
	element.setAttribute(getNamespacePrefix(Namespace::XSI), getNamespace(Namespace::XSI));
	element.setAttribute(getNamespacePrefix(Namespace::PAOS), getNamespace(Namespace::PAOS));
	element.setAttribute(getNamespacePrefix(Namespace::ADDRESSING), getNamespace(Namespace::ADDRESSING));
	element.setAttribute(getNamespacePrefix(Namespace::DSS), getNamespace(Namespace::DSS));
	element.setAttribute(getNamespacePrefix(Namespace::ECARD), getNamespace(Namespace::ECARD));
	element.setAttribute(getNamespacePrefix(Namespace::TECHSCHEMA), getNamespace(Namespace::TECHSCHEMA));

	element.appendChild(createHeaderElement(pRelatesTo, pMessageID));
	element.appendChild(createBodyElement(pBody));

	return element;
}


QDomElement PaosCreator::createResultElement(const ResponseType& pResponse)
{
	QDomElement element = mDoc.createElement(QStringLiteral("Result"));
	element.setAttribute(getNamespacePrefix(Namespace::DEFAULT), getNamespace(Namespace::DSS));

	const Result& result = pResponse.getResult();
	element.appendChild(createTextElement(QStringLiteral("ResultMajor"), result.getMajorString()));
	if (result.getMinor() != GlobalStatus::Code::No_Error)
	{
		element.appendChild(createTextElement(QStringLiteral("ResultMinor"), result.getMinorString()));
	}

	if (!result.getMessage().isNull())
	{
		QDomElement resultElement = createTextElement(QStringLiteral("ResultMessage"), result.getMessage());
		resultElement.setAttribute(QStringLiteral("xml:lang"), result.getMessageLang());
		element.appendChild(resultElement);
	}

	return element;
}
