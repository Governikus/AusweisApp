/*!
 * \brief Helper static methods for \ref test_MsgHandlerEnterPin, \ref test_MsgHandlerEnterCan and \ref test_MsgHandlerEnterPuk
 *
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "MessageDispatcher.h"

#include <QByteArray>
#include <QString>

namespace governikus
{

void setValidState(MessageDispatcher& pDispatcher,
		bool pSelectReader,
		bool pBasicReader,
		const PacePasswordId pPasswordID);

QByteArray addReaderData(const char* pData, bool pKeyPad = false);


} // namespace governikus
