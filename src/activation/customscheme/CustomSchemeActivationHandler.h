/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
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

	private Q_SLOTS:
		void onCustomUrl(const QUrl& pUrl);

	public:
		CustomSchemeActivationHandler() = default;
		virtual ~CustomSchemeActivationHandler() override = default;

		virtual bool start() override;
		virtual void stop() override;
};

} // namespace governikus
