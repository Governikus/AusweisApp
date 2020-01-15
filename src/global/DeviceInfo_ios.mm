/*!
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

#include "DeviceInfo.h"

#import <Foundation/Foundation.h>
#import <sys/utsname.h>

using namespace governikus;

QString DeviceInfo::getName()
{
	struct utsname systemInfo;
	uname(&systemInfo);
	return QString::fromNSString([NSString stringWithCString:systemInfo.machine encoding:NSUTF8StringEncoding]);
}
