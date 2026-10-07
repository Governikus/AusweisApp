/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

#include "TestParserHelper.h"

#include "TestFileHelper.h"


using namespace governikus;


QSharedPointer<ElementParser> TestParserHelper::create(const QByteArray& pContent)
{
	return QSharedPointer<ElementParser>::create(QSharedPointer<QXmlStreamReader>::create(pContent), true);
}


QSharedPointer<ElementParser> TestParserHelper::create(const QString& pFile)
{
	return create(TestFileHelper::readFile(pFile));
}
