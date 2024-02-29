/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

#include "Template.h"

#include <QFileInfo>
#include <QLoggingCategory>
#include <QRegularExpression>


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(webservice)


Template Template::fromFile(const QString& pTemplateFileName)
{
	QFile templateFile(pTemplateFileName);
	QString fileContent;
	if (!templateFile.exists())
	{
		qCCritical(webservice) << "Template file not found:" << pTemplateFileName;
	}
	else if (!templateFile.open(QFile::OpenModeFlag::ReadOnly))
	{
		qCCritical(webservice) << "Template file not openable:" << pTemplateFileName;
	}
	else
	{
		fileContent = QString::fromUtf8(templateFile.readAll());
	}
	return Template(fileContent);
}


Template::Template(const QString& pTemplate)
	: mTemplate(pTemplate)
	, mKeys()
	, mContext()
{
	const QRegularExpression keyRegEx(QStringLiteral("(\\$\\{(?<key>[^\\$\\{\\}]*)\\})"));
	auto matches = keyRegEx.globalMatch(mTemplate);
	while (matches.hasNext())
	{
		const QString& match = matches.next().captured(QStringLiteral("key"));
		if (!mKeys.contains(match))
		{
			mKeys += match;
		}
	}
}


const QSet<QString>& Template::getContextKeys() const
{
	return mKeys;
}


bool Template::setContextParameter(const QString& pKey, const QString& pValue)
{
	if (!mKeys.contains(pKey))
	{
		qCWarning(webservice) << "Ignoring unknown key:" << pKey;
		return false;
	}
	else if (mContext.contains(pKey))
	{
		qCWarning(webservice) << "Overwriting already set key:" << pKey;
	}
	mContext[pKey] = pValue;
	return true;
}


QString Template::render() const
{
	QString output = mTemplate;
	for (const auto& key : mKeys)
	{
		const auto& value = mContext.value(key);
		if (value.isNull())
		{
			qCDebug(webservice) << "No parameter specified, replace with empty string:" << key;
		}
		output = output.replace(QStringLiteral("${%1}").arg(key), value);
	}
	return output;
}
