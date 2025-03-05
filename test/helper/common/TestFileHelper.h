/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QByteArray>
#include <QDir>
#include <QSignalSpy>

namespace governikus
{

class TestFileHelper
{
	public:
		static QByteArray readFile(const QString& pFileName, bool pFromHex = false);
		static void createTranslations(const QString& pTranslationDir);
		static bool containsLog(const QSignalSpy& pSpy, const QLatin1String pStr);
		static int getUnprivilegedPortStart();
		static bool systemAllowsPort(int pPort);
};

} // namespace governikus
