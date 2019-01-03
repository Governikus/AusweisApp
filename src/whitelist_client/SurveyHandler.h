/*!
 * \brief Class holding information about an Android device to be sent to
 * the whitelist server.
 *
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Survey.h"

namespace governikus
{
class SurveyHandler
{
	private:
		Survey createSurvey(int pMaximumNfcPackageLength) const;
		void transmitSurvey(const Survey& pSurvey) const;

	public:
		SurveyHandler();

		void sendSurvey(int pMaximumNfcPackageLength) const;
};

} // namespace governikus
