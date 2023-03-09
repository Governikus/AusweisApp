/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "WifiInfo.h"

using namespace governikus;

WifiInfo::WifiInfo()
	: QObject()
	, mWifiEnabled(true)
{
	// Since iOS doesn't allow us to check the wifi state, but only allows us
	// to check if we are connected to the internet via wifi, we have some
	// edge cases where to the user not able to use the SaC. So assume
	// that SaC/Wifi just works and always set mWifiEnabled to true.
}


bool WifiInfo::getCurrentWifiEnabled()
{
	return mWifiEnabled;
}


bool WifiInfo::isWifiEnabled()
{
	return getCurrentWifiEnabled();
}


#include "moc_WifiInfo.cpp"
