/*!
 * \brief Reference information for files on smart cards.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QByteArray>

namespace governikus
{

struct FileRef
{
	FileRef(int pType, const QByteArray& pPath);

	const int type;
	const QByteArray path;

	static FileRef masterFile();
	static FileRef efDir();
	static FileRef efCardAccess();
	static FileRef efCardSecurity();
	static FileRef appESign();
};


} /* namespace governikus */
