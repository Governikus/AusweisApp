/**
 * Copyright (c) 2026 Governikus GmbH & Co. KG, Germany
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
