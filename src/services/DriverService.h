/*!
 * DriverService.h
 *
 * \brief Service providing an update mechanism for the reader driver settings
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */


#pragma once


#include "DriverSettings.h"
#include "UpdateService.h"


namespace governikus
{
class DriverService
	: public UpdateService
{
	Q_OBJECT

	private:
		static DriverService* mSharedInstance;

	public:
		DriverService(DriverSettings& pSettings);

		virtual ~DriverService();

		/*!
		 * Get the singleton
		 */
		static DriverService* getSharedInstance();


};

} /* namespace governikus */
