/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

#include "FileCopy.h"

#include <QFile>

using namespace governikus;

bool FileCopy::copyFile(const QString& pSource, const QString& pDest)
{
	return QFile::copy(pSource, pDest);
}
