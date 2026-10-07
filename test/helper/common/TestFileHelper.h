/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QSharedPointer>
#include <QSignalSpy>

namespace governikus
{

class TestFileHelper
{
	public:
		static QSharedPointer<QFile> getFile(const QString& pFileName);
		static QByteArray readFile(const QString& pFileName, bool pFromHex = false);
		static void createTranslations(const QString& pTranslationDir);
		static bool containsLog(const QSignalSpy& pSpy, const QLatin1String pStr);
		static int getUnprivilegedPortStart();
		static bool systemAllowsPort(int pPort);
};

} // namespace governikus
