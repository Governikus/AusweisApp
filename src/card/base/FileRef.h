/*!
 * \brief Reference information for files on smart cards.
 *
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QByteArray>

namespace governikus
{

struct FileRef
{
	FileRef(char pType, const QByteArray& pPath);

	const char type;
	const QByteArray path;

	static FileRef masterFile();
	static FileRef efDir();
	static FileRef efCardAccess();
	static FileRef efCardSecurity();
	static FileRef appESign();
	static FileRef appEId();
	static FileRef appPassport();
	static FileRef appPersosim();
};


} // namespace governikus
