/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CommandApduBuilder.h"
#include "FileRef.h"

namespace governikus
{

class SelectBuilder
	: public CommandApduBuilder
{
	private:
		const FileRef mFileRef;

	public:
		enum class P1 : char
		{
			SELECT_MF = 0x00, CHILD_DF = 0x01, CHILD_EF = 0x02, PARENT_DF = 0x03, APPLICATION_ID = 0x04, ABS_PATH = 0x08, REL_PATH = 0x09,
		};

		enum class P2 : char
		{
			FCI = 0x00, FCP = 0x04, FMD = 0x08, NONE = 0x0c,
		};

		explicit SelectBuilder(const FileRef& pFileRef);
		CommandApdu build() override;
};

} // namespace governikus
