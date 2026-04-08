/**
 * Copyright (c) 2026 Governikus GmbH & Co. KG, Germany
 */

#include "FileCopy.h"

#include <QFile>

using namespace governikus;

bool FileCopy::copyFile(const QString& pSource, const QString& pDest)
{
	auto sourceFile = QFile(pSource);
	if (!sourceFile.open(QIODevice::ReadOnly))
	{
		return false;
	}

	auto destFile = QFile(pDest);
	if (!destFile.open(QIODevice::WriteOnly))
	{
		return false;
	}

	const qint64 bufferSize = 4096;
	while (!sourceFile.atEnd())
	{
		const auto buffer = sourceFile.read(bufferSize);
		if (destFile.write(buffer) == -1)
		{
			return false;
		}
	}
	return true;
}
