/**
 * Copyright (c) 2026 Governikus GmbH & Co. KG, Germany
 */

#include "FileCopy.h"

#include <QFile>

using namespace governikus;

bool FileCopy::copyFile(const QString& pSource, const QString& pDest)
{
	return QFile::copy(pSource, pDest);
}
