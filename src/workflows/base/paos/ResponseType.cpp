/*!
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "ResponseType.h"

using namespace governikus;


ResponseType::ResponseType(PaosType pType)
	: PaosMessage(pType)
	, mResult(ECardApiResult::createOk())
{
}


ResponseType::~ResponseType() = default;


const ECardApiResult& ResponseType::getResult() const
{
	return mResult;
}


void ResponseType::setResult(const ECardApiResult& result)
{
	mResult = result;
}
