/*!
 * \brief Helper for some file functions.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QByteArray>
#include <QDir>

namespace governikus
{

class TestFileHelper
{
	public:
		static QByteArray readFile(const QString& pFileName);
		static void createTranslations(const QString& pTranslationDir);
};

} /* namespace governikus */
