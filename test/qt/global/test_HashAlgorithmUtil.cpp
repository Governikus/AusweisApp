/*!
 * test_HashAlgorithmUtil.cpp
 *
 * \brief Unit tests for \ref result
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "HashAlgorithmUtil.h"

#include "LogHandler.h"

#include <QtTest/QtTest>

using namespace governikus;

class test_HashAlgorithmUtil
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
#if QT_VERSION < QT_VERSION_CHECK(5, 8, 0)

		void initTestCase()
		{
			LogHandler::getInstance().init();
		}


		void cleanup()
		{
			LogHandler::getInstance().resetBacklog();
		}


		void names()
		{
			QCOMPARE(HashAlgorithmUtil::getName(QCryptographicHash::Md4), QString("MD4"));
			QCOMPARE(HashAlgorithmUtil::getName(QCryptographicHash::Md5), QString("MD5"));
			QCOMPARE(HashAlgorithmUtil::getName(QCryptographicHash::Sha1), QString("SHA1"));
			QCOMPARE(HashAlgorithmUtil::getName(QCryptographicHash::Sha224), QString("SHA224"));
			QCOMPARE(HashAlgorithmUtil::getName(QCryptographicHash::Sha256), QString("SHA256"));
			QCOMPARE(HashAlgorithmUtil::getName(QCryptographicHash::Sha384), QString("SHA384"));
			QCOMPARE(HashAlgorithmUtil::getName(QCryptographicHash::Sha512), QString("SHA512"));
			QCOMPARE(HashAlgorithmUtil::getName(QCryptographicHash::Sha3_224), QString("SHA3_224"));
			QCOMPARE(HashAlgorithmUtil::getName(QCryptographicHash::Sha3_256), QString("SHA3_256"));
			QCOMPARE(HashAlgorithmUtil::getName(QCryptographicHash::Sha3_384), QString("SHA3_384"));
			QCOMPARE(HashAlgorithmUtil::getName(QCryptographicHash::Sha3_512), QString("SHA3_512"));
		}


		void debugStream()
		{
			QSignalSpy spy(&LogHandler::getInstance(), &LogHandler::fireLog);
			QCryptographicHash::Algorithm algo = QCryptographicHash::Sha3_224;
			qDebug() << "output" << algo;

			QCOMPARE(spy.count(), 1);
			auto param = spy.takeFirst();
			QVERIFY(param.at(0).toString().contains("output SHA3_224"));
		}


#endif


};

QTEST_GUILESS_MAIN(test_HashAlgorithmUtil)
#include "test_HashAlgorithmUtil.moc"
