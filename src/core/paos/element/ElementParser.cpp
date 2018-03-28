/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "ElementParser.h"

using namespace governikus;

ElementParser::ElementParser(QSharedPointer<QXmlStreamReader> pXmlReader)
	: mXmlReader(pXmlReader)
	, mParseError(false)
{

}


ElementParser::~ElementParser()
{
}


bool ElementParser::readNextStartElement()
{
	return !mParseError && mXmlReader->readNextStartElement();
}


QString ElementParser::readElementText()
{
	QString text;

	while (mXmlReader->error() == QXmlStreamReader::NoError && !mXmlReader->isEndElement())
	{
		if (mXmlReader->readNext() == QXmlStreamReader::TokenType::Characters && !mXmlReader->isWhitespace())
		{
			text += QLatin1Char(' ');
			text += mXmlReader->text();
		}
	}

	if (mXmlReader->error() != QXmlStreamReader::NoError)
	{
		return QString();
	}

	return text.isEmpty() ? QLatin1String("") : text.simplified();
}


bool ElementParser::assertNoDuplicateElement(bool pNotYetSeen)
{
	if (!pNotYetSeen)
	{
		qCWarning(paos) << "Duplicate unique element:" << mXmlReader->name();
		mParseError = true;
	}

	return pNotYetSeen;
}


void ElementParser::assertMandatoryElement(const QString& pValue, const char* pElementName)
{
	if (pValue.isNull())
	{
		qCWarning(paos) << "Mandatory element is null:" << pElementName;
		mParseError = true;
	}
}


bool ElementParser::readUniqueElementText(QString& pText)
{
	if (!assertNoDuplicateElement(pText.isNull()))
	{
		return false;
	}

	pText = readElementText();
	return !pText.isNull();
}
