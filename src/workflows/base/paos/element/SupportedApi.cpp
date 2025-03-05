/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "SupportedApi.h"

constexpr uint SUPPORTED_ECARD_MAJOR = 1;
constexpr uint SUPPORTED_ECARD_MINOR = 1;
constexpr uint SUPPORTED_ECARD_SUBMINOR = 5;

using namespace governikus;

QString SupportedAPI::getMajor() const
{
	return QString::number(SUPPORTED_ECARD_MAJOR);
}


QString SupportedAPI::getMinor() const
{
	return QString::number(SUPPORTED_ECARD_MINOR);
}


QString SupportedAPI::getSubminor() const
{
	return QString::number(SUPPORTED_ECARD_SUBMINOR);
}
