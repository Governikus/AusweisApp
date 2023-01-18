/*!
 * \brief Msg wrapper of JSON API.
 *
 * \copyright Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "MsgTypes.h"

namespace governikus
{
class MsgHandler;

class Msg final
{
	friend class MsgHandler;

	private:
		MsgType mType;
		QByteArray mData;

		Msg(const MsgType& pType, const QByteArray& pData);

	public:
		Msg();
		operator QByteArray() const;
		operator MsgType() const;
		explicit operator bool() const;
};

char* toString(const Msg& pMsg);

} // namespace governikus
