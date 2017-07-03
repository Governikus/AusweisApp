/*!
 * ProviderService.h
 *
 * \brief Service providing an update mechanism for the provider settings
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */


#pragma once


#include "ProviderSettings.h"
#include "UpdateService.h"


namespace governikus
{
class ProviderService
	: public UpdateService
{
	Q_OBJECT

	private:
		static ProviderService* mSharedInstance;

	public:
		ProviderService(ProviderSettings& pSettings);

		virtual ~ProviderService();

		/*!
		 * Get the singleton
		 */
		static ProviderService* getSharedInstance();


};

} /* namespace governikus */
