/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#include "PinResetInformationModel.h"

#include "Env.h"
#include "ProviderConfiguration.h"

#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


Q_DECLARE_METATYPE(QMetaMethod)


class test_PinResetInformationModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_fireUpdateOnTranslationChanged()
		{
			auto pinResetInformationModel = Env::getSingleton<PinResetInformationModel>();
			QSignalSpy spy(pinResetInformationModel, &PinResetInformationModel::fireUpdated);

			pinResetInformationModel->onTranslationChanged();

			QCOMPARE(spy.count(), 1);
		}


		void test_fireUpdateOnProviderConfigurationUpdated()
		{
			QSignalSpy spy(Env::getSingleton<PinResetInformationModel>(), &PinResetInformationModel::fireUpdated);

			Q_EMIT Env::getSingleton<ProviderConfiguration>()->fireUpdated();

			QCOMPARE(spy.count(), 1);
		}


		void test_PinResetUrl()
		{
			QVERIFY(!Env::getSingleton<PinResetInformationModel>()->getPinResetUrl().isEmpty());
		}


		void test_PinResetActivationUrl()
		{
			QVERIFY(!Env::getSingleton<PinResetInformationModel>()->getPinResetActivationUrl().isEmpty());
		}


		void test_AdministrativeSearchUrl()
		{
			QVERIFY(!Env::getSingleton<PinResetInformationModel>()->getAdministrativeSearchUrl().isEmpty());
		}


		void test_HasPinResetService()
		{
			auto* config = Env::getSingleton<ProviderConfiguration>();

			config->parseProviderConfiguration(":/updatable-files/supported-providers_no-prs.json"_L1);
			QVERIFY(!Env::getSingleton<PinResetInformationModel>()->hasPinResetService());

			config->parseProviderConfiguration(":/updatable-files/supported-providers_prs.json"_L1);
			QVERIFY(Env::getSingleton<PinResetInformationModel>()->hasPinResetService());
		}


		void test_getNoPinAndNoPukHint()
		{
			auto* config = Env::getSingleton<ProviderConfiguration>();

			config->parseProviderConfiguration(":/updatable-files/supported-providers_no-prs.json"_L1);
			const auto& noPrs = Env::getSingleton<PinResetInformationModel>()->getNoPinAndNoPukHint();
			QVERIFY(!noPrs.isNull());

			config->parseProviderConfiguration(":/updatable-files/supported-providers_prs.json"_L1);
			const auto& prs = Env::getSingleton<PinResetInformationModel>()->getNoPinAndNoPukHint();
			QVERIFY(!prs.isNull());

			QVERIFY(noPrs != prs);
		}


		void test_getRequestNewPinHint()
		{
			auto* config = Env::getSingleton<ProviderConfiguration>();

			config->parseProviderConfiguration(":/updatable-files/supported-providers_no-prs.json"_L1);
			const auto& noPrs = Env::getSingleton<PinResetInformationModel>()->getRequestNewPinHint();
			QVERIFY(!noPrs.isNull());

			config->parseProviderConfiguration(":/updatable-files/supported-providers_prs.json"_L1);
			const auto& prs = Env::getSingleton<PinResetInformationModel>()->getRequestNewPinHint();
			QVERIFY(!prs.isNull());

			QVERIFY(noPrs != prs);
		}


		void test_getActivateOnlineFunctionHint()
		{
			auto* config = Env::getSingleton<ProviderConfiguration>();

			config->parseProviderConfiguration(":/updatable-files/supported-providers_no-prs.json"_L1);
			const auto& noPrs = Env::getSingleton<PinResetInformationModel>()->getActivateOnlineFunctionHint();
			QVERIFY(!noPrs.isNull());

			config->parseProviderConfiguration(":/updatable-files/supported-providers_prs.json"_L1);
			const auto& prs = Env::getSingleton<PinResetInformationModel>()->getActivateOnlineFunctionHint();
			QVERIFY(!prs.isNull());

			QVERIFY(noPrs != prs);
		}


		void test_getActivateOnlineFunctionActionText()
		{
			auto* config = Env::getSingleton<ProviderConfiguration>();

			config->parseProviderConfiguration(":/updatable-files/supported-providers_no-prs.json"_L1);
			const auto& noPrs = Env::getSingleton<PinResetInformationModel>()->getActivateOnlineFunctionActionText();
			QVERIFY(!noPrs.isNull());

			config->parseProviderConfiguration(":/updatable-files/supported-providers_prs.json"_L1);
			const auto& prs = Env::getSingleton<PinResetInformationModel>()->getActivateOnlineFunctionActionText();
			QVERIFY(!prs.isNull());

			QVERIFY(noPrs != prs);
		}


		void test_getPinResetHintNoPin()
		{
			auto* config = Env::getSingleton<ProviderConfiguration>();

			config->parseProviderConfiguration(":/updatable-files/supported-providers_no-prs.json"_L1);
			const auto& noPrs = Env::getSingleton<PinResetInformationModel>()->getPinResetHintNoPin();
			QVERIFY(!noPrs.isNull());

			config->parseProviderConfiguration(":/updatable-files/supported-providers_prs.json"_L1);
			const auto& prs = Env::getSingleton<PinResetInformationModel>()->getPinResetHintNoPin();
			QVERIFY(!prs.isNull());

			QVERIFY(noPrs != prs);
		}


		void test_getPinResetHintTransportPin()
		{
			auto* config = Env::getSingleton<ProviderConfiguration>();

			config->parseProviderConfiguration(":/updatable-files/supported-providers_no-prs.json"_L1);
			const auto& noPrs = Env::getSingleton<PinResetInformationModel>()->getPinResetHintTransportPin();
			QVERIFY(!noPrs.isNull());

			config->parseProviderConfiguration(":/updatable-files/supported-providers_prs.json"_L1);
			const auto& prs = Env::getSingleton<PinResetInformationModel>()->getPinResetHintTransportPin();
			QVERIFY(!prs.isNull());

			QVERIFY(noPrs != prs);
		}


		void test_getPinResetHint()
		{
			auto* config = Env::getSingleton<ProviderConfiguration>();

			config->parseProviderConfiguration(":/updatable-files/supported-providers_no-prs.json"_L1);
			const auto& noPrs = Env::getSingleton<PinResetInformationModel>()->getPinResetHint();
			QVERIFY(!noPrs.isNull());

			config->parseProviderConfiguration(":/updatable-files/supported-providers_prs.json"_L1);
			const auto& prs = Env::getSingleton<PinResetInformationModel>()->getPinResetHint();
			QVERIFY(!prs.isNull());

			QVERIFY(noPrs != prs);
		}


		void test_getPinResetActionText()
		{
			auto* config = Env::getSingleton<ProviderConfiguration>();

			config->parseProviderConfiguration(":/updatable-files/supported-providers_no-prs.json"_L1);
			const auto& noPrs = Env::getSingleton<PinResetInformationModel>()->getPinResetActionText();
			QVERIFY(!noPrs.isNull());

			config->parseProviderConfiguration(":/updatable-files/supported-providers_prs.json"_L1);
			const auto& prs = Env::getSingleton<PinResetInformationModel>()->getPinResetActionText();
			QVERIFY(!prs.isNull());

			QVERIFY(noPrs != prs);
		}


};

QTEST_GUILESS_MAIN(test_PinResetInformationModel)
#include "test_PinResetInformationModel.moc"
