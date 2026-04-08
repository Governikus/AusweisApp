/**
 * Copyright (c) 2014-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "paos/ElementDetector.h"
#include "paos/ResponseType.h"

#include <QByteArray>
#include <QString>

namespace governikus
{

class StartPaosResponse
	: public ResponseType
	, private ElementDetector
{
	private:
		QString mResultMajor;
		QString mResultMinor;
		QString mResultMessage;

	public:
		explicit StartPaosResponse(const QByteArray& pXmlData);

	private:
		void parse();
		bool handleFoundElement(QStringView pElementName, const QString& pValue, const QXmlStreamAttributes& pAttributes) override;
};

} // namespace governikus
