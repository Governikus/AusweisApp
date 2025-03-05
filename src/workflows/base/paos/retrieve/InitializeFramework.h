/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "paos/ElementDetector.h"
#include "paos/PaosMessage.h"

namespace governikus
{

class InitializeFramework
	: public PaosMessage
	, public ElementDetector
{
	private:
		void parse();
		bool handleFoundElement(QStringView pElementName, const QString& pValue, const QXmlStreamAttributes& pAttributes) override;

	public:
		explicit InitializeFramework(const QByteArray& pXmlData);
};

} // namespace governikus
