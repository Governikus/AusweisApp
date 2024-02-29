/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

#include "IfdDescriptor.h"

#include "messages/Discovery.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_IfdDescriptor
	: public QObject
{
	Q_OBJECT

	private:
		quint16 mPort = 0;

	private Q_SLOTS:
		void initTestCase()
		{
			mPort = 0;
		}


		void testValidDescriptorIsEqualToItself()
		{
			const Discovery validMsg(QStringLiteral("Device"), QStringLiteral("0123456789ABCDEF"), mPort, {IfdVersion::Version::latest});
			const QHostAddress address(QHostAddress::LocalHost);
			const IfdDescriptor valid(validMsg, address);

			QVERIFY(valid == valid);
		}


		void testDistinctInvalidDescriptorsAreEqual()
		{
			const QHostAddress address1(QStringLiteral("192.168.1.1"));
			const QHostAddress address2(QHostAddress::LocalHost);

			const IfdDescriptor invalid1(Discovery(QJsonObject()), address1);
			const IfdDescriptor invalid2(Discovery(QJsonObject()), address2);

			QVERIFY(invalid1 == invalid2);
		}


		void testValidDescriptorIsDifferentFromInvalid()
		{
			const Discovery validMsg(QStringLiteral("Device"), QStringLiteral("0123456789ABCDEF"), mPort, {IfdVersion::Version::latest});
			const Discovery invalidMsg(""_L1, ""_L1, 0, {});
			const QHostAddress address(QHostAddress::LocalHost);

			const IfdDescriptor valid(validMsg, address);
			const IfdDescriptor invalid(invalidMsg, address);

			QVERIFY(!(valid == invalid));
		}


		void testDistinctValidDescriptorsWithDifferentDataAreDifferent()
		{
			const Discovery validMsg1(QStringLiteral("Device"), QStringLiteral("0123456789ABCDEF"), mPort, {IfdVersion::Version::latest});
			const Discovery validMsg2(QStringLiteral("Device"), QStringLiteral("0123456789ABCDEF"), mPort, {IfdVersion::Version::latest, IfdVersion::Version::v2});
			const QHostAddress address(QHostAddress::LocalHost);

			const IfdDescriptor valid1(validMsg1, address);
			const IfdDescriptor valid2(validMsg2, address);

			QVERIFY(!(valid1 == valid2));
		}


		void testDistinctValidDescriptorsWithTheSameDataAreEqual()
		{
			const Discovery validMsg1(QStringLiteral("Device"), QStringLiteral("0123456789ABCDEF"), mPort, {IfdVersion::Version::latest});
			const Discovery validMsg2(QStringLiteral("Device"), QStringLiteral("0123456789ABCDEF"), mPort, {IfdVersion::Version::latest});
			const QHostAddress address(QHostAddress::LocalHost);

			const IfdDescriptor valid1(validMsg1, address);
			const IfdDescriptor valid2(validMsg2, address);

			QVERIFY(valid1 == valid2);
		}


};

QTEST_GUILESS_MAIN(test_IfdDescriptor)
#include "test_IfdDescriptor.moc"
