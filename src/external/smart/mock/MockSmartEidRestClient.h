/*
 * \copyright Copyright (c) 2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QByteArray>
#include <QStringList>
#include <QUrl>
#include <QUuid>

#include <eid_applet_results.h>
#include <string>

namespace governikus
{

class MockSmartEidRestClient
{
	private:
		static const char* SMART_EID_MOCK_URL;

		const QUrl mBaseUrl;
		QUuid mSessionId;

		static QUuid parseSessionId(const QByteArray& pJsonData);
		static std::string parsePreparePersonalizationData(const QByteArray& pJsonData);
		static QStringList parseCommandList(const QByteArray& pJsonData);

	public:
		MockSmartEidRestClient();

		InitializeResult createSession();
		PersonalizationResult deleteSession();
		GenericDataResult sendPersonalizationCommand(const std::string& pCommand) const;
		bool isEnabled() const;
};

} // namespace governikus
