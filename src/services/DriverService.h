/*!
 * \brief Service providing an update mechanism for the reader driver settings
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "UpdateService.h"

namespace governikus
{
class DriverService
	: public UpdateService
{
	Q_OBJECT

	protected:
		DriverService();
		virtual ~DriverService();

	public:
		static DriverService& getInstance();
};

} /* namespace governikus */
