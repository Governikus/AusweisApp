/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/**
 * \brief Example class
 */

#pragma once

#include <QByteArray>
#include <QStringList>
#include <QXmlStreamReader>

namespace governikus
{

class ElementDetector
{
	Q_DISABLE_COPY(ElementDetector)

	private:
		QXmlStreamReader mReader;

	protected:
		void handleStartElements(const QStringList& pStartElementNames);
		void detectStartElements(const QStringList& pStartElementNames);
		virtual bool handleFoundElement(const QString& pElementName, const QString& pValue, const QXmlStreamAttributes& pAttributes) = 0;

	public:
		explicit ElementDetector(const QByteArray& pXmlData);
		virtual ~ElementDetector();
};

} // namespace governikus
