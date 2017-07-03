/*!
 * ElementDetector.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "ElementDetector.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(paos)


ElementDetector::ElementDetector(const QByteArray& pXmlData)
	: mReader(pXmlData)
	, mXmlData(pXmlData)
{
}


ElementDetector::~ElementDetector()
{
}


void ElementDetector::detectStartElements(const QStringList& pStartElementNames)
{
	for (; !mReader.atEnd(); mReader.readNext())
	{
		if (mReader.hasError())
		{
			qCWarning(paos) << "Error parsing PAOS message: " << mReader.errorString();
			return;
		}
		else if (mReader.isStartElement())
		{
			handleStartElements(pStartElementNames);
		}
	}
}


void ElementDetector::handleStartElements(const QStringList& pStartElementNames)
{
	QString name = mReader.name().toString();
	if (pStartElementNames.contains(name))
	{
		QXmlStreamAttributes attributes = mReader.attributes();
		QString value;
		if (mReader.readNext() == QXmlStreamReader::TokenType::Characters && !mReader.isWhitespace())
		{
			value = mReader.text().toString().simplified();
		}
		if (!handleFoundElement(name, value, attributes))
		{
			return;
		}
	}
}
