/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "Commands.h"
#include "FileRef.h"

using namespace governikus;

FileRef FileRef::efDir()
{
	return FileRef(static_cast<char>(SelectBuilder::P1::CHILD_EF), QByteArray::fromHex("2f00"));
}


FileRef FileRef::masterFile()
{
	return FileRef(static_cast<char>(SelectBuilder::P1::SELECT_MF), QByteArray::fromHex("3f00"));
}


FileRef FileRef::efCardAccess()
{
	/*
	 * File ID 011C specified in TR-03110-3
	 */
	return FileRef(static_cast<char>(SelectBuilder::P1::CHILD_EF), QByteArray::fromHex("011c"));
}


FileRef FileRef::efCardSecurity()
{
	/*
	 * File ID 011D specified in TR-03110-3
	 */
	return FileRef(static_cast<char>(SelectBuilder::P1::CHILD_EF), QByteArray::fromHex("011d"));
}


FileRef FileRef::appESign()
{
	return FileRef(static_cast<char>(SelectBuilder::P1::APPLICATION_ID), QByteArray::fromHex("a000000167455349474e"));
}


FileRef::FileRef(char pType, const QByteArray& pPath)
	: type(pType)
	, path(pPath)
{
}
