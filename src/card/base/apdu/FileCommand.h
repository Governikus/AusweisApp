/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
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
		int mOffset;
		int mLe;

	public:
		explicit FileCommand(const CommandApdu& pCommandApdu);
		explicit FileCommand(const FileRef& pFileRef, int pOffset = 0, int pLe = CommandApdu::NO_LE);

		[[nodiscard]] const FileRef& getFileRef() const;
		[[nodiscard]] int getOffset() const;
		[[nodiscard]] int getLe() const;

		operator CommandApdu() const;
};

} // namespace governikus
