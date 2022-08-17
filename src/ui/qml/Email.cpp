/*!
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

#include "Email.h"

#include "BuildHelper.h"
#include "GlobalStatus.h"
#include "LogHandler.h"

namespace governikus
{

QString generateMailBody(const GlobalStatus& pStatus, const QUrl& pServiceUrl, bool pPercentEncoding, bool pAddLogNotice)
{
	const auto& logHandler = Env::getSingleton<LogHandler>();
	QStringList mailBody(QObject::tr("Please describe the error that occurred."));

	if (logHandler->useLogFile() && pAddLogNotice)
	{
		mailBody << QObject::tr("You may want to attach the logfile which can be saved from the error dialog.");
	}

	const QLatin1String newLine("\n");
	mailBody << newLine;

	const auto& systemInfo = BuildHelper::getInformationHeader();
	for (const auto& info : systemInfo)
	{
		QString first = info.first;
		QString second = info.second;
		if (pPercentEncoding)
		{
			first = QString::fromUtf8(QUrl::toPercentEncoding(first));
			second = QString::fromUtf8(QUrl::toPercentEncoding(second));

		}
		mailBody << first + QStringLiteral(": ") + second;
	}

	mailBody << newLine + QObject::tr("Error code") + QLatin1Char(':');
	mailBody << getEnumName(pStatus.getStatusCode());

	if (!pServiceUrl.isEmpty())
	{
		const auto& serviceUrlStub = pServiceUrl.scheme() + QStringLiteral("://") + pServiceUrl.authority();
		mailBody << newLine + QObject::tr("Service URL") + QLatin1Char(':') << serviceUrlStub;
	}

	mailBody << newLine + QObject::tr("Parameter of occurred error:") << pStatus.getExternalInfo(newLine);

	if (logHandler->hasCriticalLog())
	{
		const QString criticalMessages = QString::fromUtf8(logHandler->getCriticalLogWindow());
		mailBody << newLine + QObject::tr("Critical errors:") + newLine + criticalMessages;
	}

	auto result = mailBody.join(newLine);
	if (pPercentEncoding)
	{
		// We need to replace the ambersand char since it will signal the end of the predefined mail body
		// and everything beyond that ambersand will be missing from the mail.
		result = result.replace(QStringLiteral("&"), QStringLiteral("%26"));
	}
	return result;
}


} // namespace governikus
