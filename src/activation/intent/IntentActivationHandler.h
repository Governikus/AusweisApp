/*!
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ActivationHandler.h"

namespace governikus
{

/*!
 * This ActivationHandler implements an API by opening custom URLs registered in the Android manifest,
 * as specified by TR-03124-1.
 * The URL is passed by an Android Intent mechanism to the application.
 */
class IntentActivationHandler
	: public ActivationHandler
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.ActivationHandler" FILE "metadata.json")
	Q_INTERFACES(governikus::ActivationHandler)

	private:
		void onIntent(const QUrl& pUrl, const QString& pReferrer);

	public:
		IntentActivationHandler() = default;

		virtual bool start() override;
		virtual void stop() override;
		virtual void onApplicationActivated() override;
};

} // namespace governikus
