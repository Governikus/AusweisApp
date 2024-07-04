/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
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
		bool handleStartElements(const QStringList& pStartElementNames);
		void detectStartElements(const QStringList& pStartElementNames);
		virtual bool handleFoundElement(QStringView pElementName, const QString& pValue, const QXmlStreamAttributes& pAttributes) = 0;

	public:
		explicit ElementDetector(const QByteArray& pXmlData);
		virtual ~ElementDetector();
};

} // namespace governikus
