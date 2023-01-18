/*!
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "FileRef.h"

#include <QString>


using namespace governikus;


FileRef FileRef::masterFile()
{
	return FileRef(TYPE::MASTER_FILE, QByteArray::fromHex("3F00"));
}


FileRef FileRef::efDir()
{
	/*
	 * File ID 2F00 / 1E specified in ICAO Doc 9303-10 3.11.2 EF.DIR (CONDITIONAL)
	 */
	return FileRef(TYPE::ELEMENTARY_FILE, QByteArray::fromHex("2F00"), QByteArray::fromHex("1E"));
}


FileRef FileRef::efCardAccess()
{
	/*
	 * File ID 011C / 1C specified in TR-03110-3 A.1.2. Storage on the Chip
	 */
	return FileRef(TYPE::ELEMENTARY_FILE, QByteArray::fromHex("011C"), QByteArray::fromHex("1C"));
}


FileRef FileRef::efCardSecurity()
{
	/*
	 * File ID 011D / 1D specified in TR-03110-3 A.1.2. Storage on the Chip
	 */
	return FileRef(TYPE::ELEMENTARY_FILE, QByteArray::fromHex("011D"), QByteArray::fromHex("1D"));
}


FileRef FileRef::appCIA()
{
	return FileRef(TYPE::APPLICATION, QByteArray::fromHex("E828BD080FA000000167455349474E"));
}


FileRef FileRef::appEId()
{
	return FileRef(TYPE::APPLICATION, QByteArray::fromHex("E80704007F00070302"));
}


FileRef FileRef::appESign()
{
	return FileRef(TYPE::APPLICATION, QByteArray::fromHex("A000000167455349474E"));
}


FileRef FileRef::appPassport()
{
	return FileRef(TYPE::APPLICATION, QByteArray::fromHex("A0000002471001"));
}


FileRef FileRef::appPersosim()
{
	return FileRef(TYPE::APPLICATION, QByteArray::fromHex("F04E66E75C02D8"));
}


FileRef::FileRef()
	: mType(TYPE::UNKNOWN)
	, mIdentifier()
{
}


FileRef::FileRef(uchar pType, const QByteArray& pIdentifier, const QByteArray& pShortIdentifier)
	: mType(TYPE::UNKNOWN)
	, mIdentifier(pIdentifier)
	, mShortIdentifier(pShortIdentifier)
{
	switch (pType)
	{
		case 0x00:
			mType = TYPE::MASTER_FILE;
			return;

		case 0x02:
			mType = TYPE::ELEMENTARY_FILE;
			return;

		case 0x04:
			mType = TYPE::APPLICATION;
			return;
	}
}


FileRef::FileRef(TYPE pType, const QByteArray& pIdentifier, const QByteArray& pShortIdentifier)
	: mType(pType)
	, mIdentifier(pIdentifier)
	, mShortIdentifier(pShortIdentifier)
{
}


FileRef::TYPE FileRef::getType() const
{
	return mType;
}


const QByteArray& FileRef::getIdentifier() const
{
	return mIdentifier;
}


const QByteArray& FileRef::getShortIdentifier() const
{
	return mShortIdentifier;
}


QString FileRef::getName() const
{
	switch (mType)
	{
		case TYPE::MASTER_FILE:
			return QStringLiteral("MasterFile");

		case TYPE::ELEMENTARY_FILE:
			return QStringLiteral("ElementaryFile");

		default:
			if (mIdentifier == appCIA().getIdentifier())
			{
				return QStringLiteral("eSign CIA");
			}

			if (mIdentifier == FileRef::appEId().getIdentifier())
			{
				return QStringLiteral("eID");
			}

			if (mIdentifier == FileRef::appESign().getIdentifier())
			{
				return QStringLiteral("eSign");
			}

			if (mIdentifier == FileRef::appPassport().getIdentifier())
			{
				return QStringLiteral("Passport");
			}

			if (mIdentifier == FileRef::appPersosim().getIdentifier())
			{
				return QStringLiteral("PersoSim");
			}
	}

	return QStringLiteral("Unknown");
}
