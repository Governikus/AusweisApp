/*!
 * \brief Helper for some file functions.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
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
		static QByteArray readFile(const QString& pFileName);
		static void createTranslations(const QString& pTranslationDir);
		static bool containsLog(const QSignalSpy& pSpy, const QLatin1String pStr);
};

} /* namespace governikus */
