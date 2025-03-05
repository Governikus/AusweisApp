/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "ElementDetector.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(paos)


ElementDetector::ElementDetector(const QByteArray& pXmlData)
	: mReader(pXmlData)
{
}


ElementDetector::~ElementDetector() = default;


void ElementDetector::detectStartElements(const QStringList& pStartElementNames)
{
	for (; !mReader.atEnd(); mReader.readNext())
	{
		if (mReader.hasError())
		{
			qCWarning(paos) << "Error parsing PAOS message:" << mReader.errorString();
			return;
		}
		else if (mReader.isStartElement() && !handleStartElements(pStartElementNames))
		{
			return;
		}
	}
}


bool ElementDetector::handleStartElements(const QStringList& pStartElementNames)
{
	const QString name = mReader.name().toString();
	if (pStartElementNames.contains(name))
	{
		QXmlStreamAttributes attributes = mReader.attributes();
		QString value;
		if (mReader.readNext() == QXmlStreamReader::TokenType::Characters && !mReader.isWhitespace())
		{
			value = mReader.text().toString().simplified();
		}

		return handleFoundElement(name, value, attributes);
	}

	return true;
}
