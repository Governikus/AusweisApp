/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CommandApdu.h"
#include "FileRef.h"

class test_Commands;

namespace governikus
{

class FileCommand
{
	private:
		FileRef mFileRef;
		qsizetype mOffset;
		int mLe;

	public:
		explicit FileCommand(const CommandApdu& pCommandApdu);
		explicit FileCommand(const FileRef& pFileRef, qsizetype pOffset = 0, int pLe = CommandApdu::NO_LE);

		[[nodiscard]] const FileRef& getFileRef() const;
		[[nodiscard]] qsizetype getOffset() const;
		[[nodiscard]] int getLe() const;

		operator CommandApdu() const;
};

} // namespace governikus
