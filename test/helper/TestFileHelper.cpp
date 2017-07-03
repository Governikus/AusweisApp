/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "TestFileHelper.h"

#include "FileDestination.h"

#include <QFile>
#include <QtTest>

using namespace governikus;

static const uchar qm_magic[16] = {
	0x3c, 0xb8, 0x64, 0x18, 0xca, 0xef, 0x9c, 0x95,
	0xcd, 0x21, 0x1c, 0xbf, 0x60, 0xa1, 0xbd, 0xdd
};

static const uchar qm_content[48] = {
	0x42, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
	0x53, 0x00, 0x00, 0x00, 0x00, 0x69, 0x00, 0x00,
	0x00, 0x17, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0x08,
	0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
	0x01, 0x53, 0x07, 0x00, 0x00, 0x00, 0x01, 0x54,
	0x01, 0x88, 0x00, 0x00, 0x00, 0x02, 0x01, 0x01
};


QByteArray TestFileHelper::readFile(const QString& pFileName)
{
	QFile file(pFileName);
	Q_ASSERT(file.exists());
	Q_ASSERT(file.open(QIODevice::ReadOnly | QIODevice::Unbuffered));
	QByteArray content = file.readAll();
	file.close();
	return content;
}


void TestFileHelper::createTranslations(const QString& pTranslationDir)
{
	const QStringList testFiles({
				"de_AT", "de_DE", "en", "fr_FR", "es", "defect", "de_fe", "en_gb", "it"
			});

	QDir dir(pTranslationDir);
	if (dir.exists())
	{
		QVERIFY(dir.removeRecursively());
	}
	QVERIFY(dir.mkpath(dir.path()));

	for (const auto& filePart : testFiles)
	{
		QFile file(dir.path() + "/ausweisapp2_" + filePart + ".qm");
		QVERIFY(file.open(QIODevice::WriteOnly));
		QVERIFY(file.write(reinterpret_cast<const char*>(qm_magic), sizeof(qm_magic)));
		QVERIFY(file.write(reinterpret_cast<const char*>(qm_content), sizeof(qm_content)));
		QVERIFY(file.exists());
	}
}


bool TestFileHelper::containsLog(const QSignalSpy& pSpy, const QLatin1String& pStr)
{
	for (const auto& entry : pSpy)
	{
		if (entry.at(0).toString().contains(pStr))
		{
			return true;
		}
	}
	return false;
}
