/*!
 * \brief Service providing an update mechanism for the provider settings
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "UpdateService.h"

namespace governikus
{
class ProviderService
	: public UpdateService
{
	Q_OBJECT

	protected:
		ProviderService();
		virtual ~ProviderService();

	public:
		static ProviderService& getInstance();
};

} /* namespace governikus */
