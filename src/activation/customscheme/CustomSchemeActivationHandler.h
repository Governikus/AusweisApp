/*!
 * CustomSchemeActivationHandler.h
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "ActivationHandler.h"

namespace governikus
{

/*!
 * This ActivationHandler implements an API by opening custom URLs with scheme 'eid',
 * as specified by TR-03124-1.
 * Custom schemes can be registered for iOS apps.
 */
class CustomSchemeActivationHandler
	: public ActivationHandler
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.ActivationHandler" FILE "metadata.json")
	Q_INTERFACES(governikus::ActivationHandler)

	protected:
		void onCustomUrl(const QUrl& pUrl);

	public:
		CustomSchemeActivationHandler();
		virtual ~CustomSchemeActivationHandler();

		virtual bool start() override;
		virtual void stop() override;
};

} /* namespace governikus */
