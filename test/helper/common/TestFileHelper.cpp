/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "TestFileHelper.h"

#include <QFile>
#include <QtTest>

#include <algorithm>

#if __has_include(<unistd.h>)
	#include <unistd.h>
#endif

using namespace governikus;

static const uchar qm_magic[16] = {
	0x3C, 0xB8, 0x64, 0x18, 0xCA, 0xEF, 0x9C, 0x95,
	0xCD, 0x21, 0x1C, 0xBF, 0x60, 0xA1, 0xBD, 0xDD
};

static const uchar qm_content[48] = {
	0x42, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
	0x53, 0x00, 0x00, 0x00, 0x00, 0x69, 0x00, 0x00,
	0x00, 0x17, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0x08,
	0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
	0x01, 0x53, 0x07, 0x00, 0x00, 0x00, 0x01, 0x54,
	0x01, 0x88, 0x00, 0x00, 0x00, 0x02, 0x01, 0x01
};


QByteArray TestFileHelper::readFile(const QString& pFileName, bool pFromHex)
{
	QFile file(pFileName);
	Q_ASSERT(file.exists());
	Q_ASSERT(file.open(QIODevice::ReadOnly | QIODevice::Unbuffered));
	QByteArray content = file.readAll();
	file.close();
	return pFromHex ? QByteArray::fromHex(content) : content;
}


void TestFileHelper::createTranslations(const QString& pTranslationDir)
{
	const QStringList testFiles({
				QStringLiteral("de_AT"), QStringLiteral("de_DE"),
				QStringLiteral("en"), QStringLiteral("fr_FR"),
				QStringLiteral("es"), QStringLiteral("defect"),
				QStringLiteral("de_fe"), QStringLiteral("en_gb"),
				QStringLiteral("it")
			});

	QDir dir(pTranslationDir);
	if (dir.exists())
	{
		QVERIFY(dir.removeRecursively());
	}
	QVERIFY(dir.mkpath(dir.path()));

	for (const auto& filePart : testFiles)
	{
		QFile file(dir.path() + QStringLiteral("/ausweisapp_") + filePart + QStringLiteral(".qm"));
		QVERIFY(file.open(QIODevice::WriteOnly));
		QVERIFY(file.write(reinterpret_cast<const char*>(qm_magic), sizeof(qm_magic)));
		QVERIFY(file.write(reinterpret_cast<const char*>(qm_content), sizeof(qm_content)));
		QVERIFY(file.exists());
	}
}


bool TestFileHelper::containsLog(const QSignalSpy& pSpy, const QLatin1String pStr)
{
	return std::any_of(pSpy.constBegin(), pSpy.constEnd(), [pStr](const auto& pEntry)
		{
			return pEntry.at(0).toString().contains(pStr);
		});
}


int TestFileHelper::getUnprivilegedPortStart()
{
	int result = -1;

#ifdef Q_OS_LINUX
	const auto data = TestFileHelper::readFile(QStringLiteral("/proc/sys/net/ipv4/ip_unprivileged_port_start")).trimmed();
	bool ok = false;
	const int converted = data.toInt(&ok); // toInt returns 0 if "ok=false"
	if (ok)
	{
		result = converted;
	}
	else
	{
		qCritical() << "Cannot convert port data";
	}
#endif

	return result;
}


bool TestFileHelper::systemAllowsPort(int pPort)
{
	return TestFileHelper::getUnprivilegedPortStart() <= pPort
#if defined(Q_OS_LINUX) || defined(Q_OS_MACOS) || defined(Q_OS_BSD4)
		   || getuid() == 0
#endif
	;
}
