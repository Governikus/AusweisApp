/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "SelectBuilder.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(card)

using namespace governikus;

/*
 * SelectBuilder
 */
SelectBuilder::SelectBuilder(const FileRef& pFileRef)
	: CommandApduBuilder()
	, mFileRef(pFileRef)
{
}


CommandApdu SelectBuilder::build()
{
	static const char INS = char(0xA4);
	return CommandApdu(CommandApdu::CLA, INS, mFileRef.type, static_cast<char>(P2::NONE), mFileRef.path);
}
