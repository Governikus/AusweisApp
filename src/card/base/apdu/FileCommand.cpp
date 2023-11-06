/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "FileCommand.h"

#include <QLoggingCategory>
#include <QtEndian>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


FileCommand::FileCommand(const CommandApdu& pCommandApdu)
	: mFileRef()
	, mOffset(0)
	, mLe(pCommandApdu.getLe())
{
	switch (pCommandApdu.getINS())
	{
		case Ins::SELECT:
			mFileRef = FileRef(pCommandApdu.getP1(), pCommandApdu.getData());
			return;

		case Ins::READ_BINARY:
		case Ins::UPDATE_BINARY:
		{
			const auto& header = pCommandApdu.getHeaderBytes();

			if (uchar p1 = pCommandApdu.getP1(); (p1 & 0xE0) == 0x80)
			{
				mFileRef = FileRef(FileRef::TYPE::ELEMENTARY_FILE, QByteArray(), QByteArray(1, p1 & 0x1F));
				mOffset = qFromBigEndian<quint8>(header.data() + 3);
				return;
			}

			mOffset = qFromBigEndian<quint16>(header.data() + 2);
			return;
		}

		default:
			Q_ASSERT(false);
			qCCritical(card) << "Unexpected INS";
	}
}


FileCommand::FileCommand(const FileRef& pFileRef, qsizetype pOffset, int pLe)
	: mFileRef(pFileRef)
	, mOffset(pOffset)
	, mLe(pLe)
{
}


const FileRef& FileCommand::getFileRef() const
{
	return mFileRef;
}


qsizetype FileCommand::getOffset() const
{
	return mOffset;
}


int FileCommand::getLe() const
{
	return mLe;
}


FileCommand::operator CommandApdu() const
{
	if (mLe == 0)
	{
		constexpr char PROPRIETARY = 0x0C;
		return CommandApdu(Ins::SELECT, mFileRef.getType(), PROPRIETARY, mFileRef.getIdentifier());
	}

	if (mOffset == 0 && mFileRef.getType() == FileRef::ELEMENTARY_FILE)
	{
		return CommandApdu(Ins::READ_BINARY, 0x80 | static_cast<uchar>(mFileRef.getShortIdentifier().at(0)), 0, QByteArray(), mLe);
	}

	const auto offset = static_cast<uint>(mOffset);
	return CommandApdu(Ins::READ_BINARY, (offset & 0xFF00) >> 8, offset & 0xFF, QByteArray(), mLe);
}
