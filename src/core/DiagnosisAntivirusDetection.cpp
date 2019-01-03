/*
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include "DiagnosisAntivirusDetection.h"

#include <QFile>
#include <QStandardPaths>

using namespace governikus;

AntivirInfo::AntivirInfo(const QString& pDisplayName, const QString& pLastUpdate, const QString& pExePath)
	: mDisplayName(pDisplayName),
	mLastUpdate(pLastUpdate),
	mExePath(pExePath)
{
}


DiagnosisAntivirusDetection::DiagnosisAntivirusDetection()
	: QObject()
#if defined(Q_OS_WIN)
	, mProcess(QSharedPointer<QProcess>::create())
#endif
	, mAntivirInfos()
{
}


void DiagnosisAntivirusDetection::startInformationProcess()
{
#if defined(Q_OS_WIN)
	QString powershellCommand = QStandardPaths::findExecutable(QStringLiteral("powershell.exe"));
	if (powershellCommand.isEmpty())
	{
		powershellCommand = QStringLiteral("C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe");
		if (!QFile::exists(powershellCommand))
		{
			onError(QProcess::ProcessError::FailedToStart);
			return;
		}
	}

	QStringList parameters;
	parameters << QStringLiteral("-NoProfile");
	parameters << QStringLiteral("-Command");
	parameters << QStringLiteral("Get-WmiObject -Namespace root\\SecurityCenter2 -Class AntiVirusProduct");

	connect(mProcess.data(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &DiagnosisAntivirusDetection::onFinished);
	connect(mProcess.data(), &QProcess::errorOccurred, this, &DiagnosisAntivirusDetection::onError);

	mProcess->start(powershellCommand, parameters);
	mProcess->closeWriteChannel();
#endif
}


#if defined(Q_OS_WIN)
void DiagnosisAntivirusDetection::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	if (exitCode != 0)
	{
		qDebug() << "Error while getting antivirus informations, error code:" << exitCode;
		Q_EMIT fireDetectionFailed();
		return;
	}

	if (exitStatus == QProcess::CrashExit)
	{
		return;
	}

	QString output = QString::fromUtf8(mProcess->readAllStandardOutput());

	QString displayName;
	QString lastUpdate;
	QString exePath;

	const auto& lines = output.split(QStringLiteral("\r\n"));
	for (const auto& line : lines)
	{
		const QString& trimmedLine = line.trimmed();
		if (trimmedLine.isEmpty())
		{
			if (!displayName.isEmpty() && !exePath.isEmpty())
			{
				auto newInfo = QSharedPointer<AntivirInfo>::create(displayName, lastUpdate, exePath);
				mAntivirInfos << newInfo;
			}
			displayName = QString();
			lastUpdate = QString();
			exePath = QString();
			continue;
		}

		const QStringList& lineparts = trimmedLine.split(QStringLiteral(" : "));
		if (lineparts.length() != 2)
		{
			continue;
		}
		if (lineparts[0].startsWith(QLatin1String("displayName")))
		{
			displayName = lineparts[1];
		}
		else if (lineparts[0].startsWith(QLatin1String("timestamp")))
		{
			lastUpdate = lineparts[1];
		}
		else if (lineparts[0].startsWith(QLatin1String("pathToSignedProductExe")))
		{
			exePath = lineparts[1];
		}
	}

	Q_EMIT fireAntivirusInformationChanged();
}


#endif


#if defined(Q_OS_WIN)
void DiagnosisAntivirusDetection::onError(QProcess::ProcessError pError)
{
	qDebug() << "Error calling process:" << pError;
	Q_EMIT fireDetectionFailed();
}


#endif


const QVector<QSharedPointer<AntivirInfo> >& DiagnosisAntivirusDetection::getAntivirusInformations() const
{
	return mAntivirInfos;
}


#include "moc_DiagnosisAntivirusDetection.cpp"
