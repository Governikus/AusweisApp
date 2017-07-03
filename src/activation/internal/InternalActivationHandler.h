/*!
 * \brief ActivationHandler for Internal usage like JSON API.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "ActivationHandler.h"
#include "InternalActivationContext.h"

namespace governikus
{

class InternalActivationHandler
	: public ActivationHandler
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.ActivationHandler" FILE "metadata.json")
	Q_INTERFACES(governikus::ActivationHandler)

	public:
		InternalActivationHandler();
		virtual ~InternalActivationHandler();

		virtual bool start() override;
		virtual void stop() override;

		void runAuthentication(InternalActivationContext* pContext);
};

} /* namespace governikus */
