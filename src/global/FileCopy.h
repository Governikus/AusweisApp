/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

#pragma once

class QString;

namespace governikus
{

struct FileCopy
{
	static bool copyFile(const QString& pSource, const QString& pDest);
};

} // namespace governikus
