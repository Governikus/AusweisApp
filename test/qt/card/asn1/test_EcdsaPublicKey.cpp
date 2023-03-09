/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref EcdsaPublicKey
 */

#include "asn1/EcdsaPublicKey.h"

#include "asn1/CVCertificate.h"
#include "asn1/Oid.h"

#include <QtTest>
#include <openssl/bn.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/x509v3.h>


using namespace governikus;


class test_EcdsaPublicKey
	: public QObject
{
	Q_OBJECT


	QByteArray convert(BIGNUM* pBigNum)
	{
		if (pBigNum == nullptr)
		{
			qWarning() << "got nullptr, returning empty QByteArray";
			return QByteArray();
		}

		QVector<uchar> buf(BN_num_bytes(pBigNum));
		BN_bn2bin(pBigNum, buf.data());
		return QByteArray(reinterpret_cast<const char*>(buf.data()), buf.size());
	}


	QByteArray fetchEcParams(const QSharedPointer<EVP_PKEY>& pKey, BIGNUM** pA, BIGNUM** pB, BIGNUM** pP, BIGNUM** pCofactor, BIGNUM** pOrder)
	{
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
		EVP_PKEY_get_bn_param(pKey.data(), "a", pA);
		EVP_PKEY_get_bn_param(pKey.data(), "b", pB);
		EVP_PKEY_get_bn_param(pKey.data(), "p", pP);
		EVP_PKEY_get_bn_param(pKey.data(), "cofactor", pCofactor);
		EVP_PKEY_get_bn_param(pKey.data(), "order", pOrder);

		QByteArray generator(1024, 0);
		size_t usedSize = 0;
		EVP_PKEY_get_octet_string_param(pKey.data(), "generator", reinterpret_cast<uchar*>(generator.data()), static_cast<size_t>(generator.size()), &usedSize);
		generator.resize(static_cast<int>(usedSize));
		return generator;

#else
		const EC_GROUP* ecGroup = EC_KEY_get0_group(EVP_PKEY_get0_EC_KEY(pKey.data()));
		EC_GROUP_get_cofactor(ecGroup, *pCofactor, nullptr);
		EC_GROUP_get_order(ecGroup, *pOrder, nullptr);

	#if OPENSSL_VERSION_NUMBER < 0x10101000L || defined(LIBRESSL_VERSION_NUMBER)
		EC_GROUP_get_curve_GFp(ecGroup, *pP, *pA, *pB, nullptr);
	#else
		EC_GROUP_get_curve(ecGroup, *pP, *pA, *pB, nullptr);
	#endif

		const EC_POINT* generator = EC_GROUP_get0_generator(ecGroup);
		auto bufLen = EC_POINT_point2oct(ecGroup, generator, point_conversion_form_t::POINT_CONVERSION_UNCOMPRESSED, nullptr, 0, nullptr);

		Q_ASSERT(bufLen <= INT_MAX);
		QVector<char> buf(static_cast<int>(bufLen));
		EC_POINT_point2oct(ecGroup, generator, point_conversion_form_t::POINT_CONVERSION_UNCOMPRESSED, reinterpret_cast<uchar*>(buf.data()), static_cast<size_t>(buf.size()), nullptr);
		return QByteArray(buf.data(), buf.size());

#endif
	}

	private Q_SLOTS:
		void parseCrap()
		{
			QByteArray hexString("304F"
								 "        06 0A 04007F00070202020203"
								 "        86 41 0426DB60E123E6B0D740E544213CD35CAB4D59D692A02A6FD97E06FE52FA32AFF05B1748141260AA618A05671E8C70C1957B2E96794EA08DEEB47834A074CF6912");

			auto ecdsaPublicKey = EcdsaPublicKey::fromHex(hexString);

			QVERIFY(ecdsaPublicKey == nullptr);
		}


		void missingOid()
		{
			QByteArray hexString("7F4943"
								 "        86 41 0426DB60E123E6B0D740E544213CD35CAB4D59D692A02A6FD97E06FE52FA32AFF05B1748141260AA618A05671E8C70C1957B2E96794EA08DEEB47834A074CF6912");

			auto ecdsaPublicKey = EcdsaPublicKey::fromHex(hexString);

			QVERIFY(ecdsaPublicKey == nullptr);
		}


		void missingPublicPoint()
		{
			QByteArray hexString("7F490C"
								 "        06 0A 04007F00070202020203");

			auto ecdsaPublicKey = EcdsaPublicKey::fromHex(hexString);

			QVERIFY(ecdsaPublicKey == nullptr);
		}


		void onlyMandatoryElements()
		{
			QByteArray hexString("7F494F"
								 "        06 0A 04007F00070202020203"
								 "        86 41 0426DB60E123E6B0D740E544213CD35CAB4D59D692A02A6FD97E06FE52FA32AFF05B1748141260AA618A05671E8C70C1957B2E96794EA08DEEB47834A074CF6912");

			auto ecdsaPublicKey = EcdsaPublicKey::fromHex(hexString);

			QVERIFY(ecdsaPublicKey != nullptr);
			QVERIFY(!ecdsaPublicKey->isComplete());
			QVERIFY(ecdsaPublicKey->createKey().isNull());
			QCOMPARE(ecdsaPublicKey->getSecurityProtocol(), SecurityProtocol(KnownOid::ID_TA_ECDSA_SHA_256));
			QCOMPARE(ecdsaPublicKey->getOid(), Oid(KnownOid::ID_TA_ECDSA_SHA_256));
			QCOMPARE(ecdsaPublicKey->getUncompressedPublicPoint().toHex().toUpper(), QByteArray("0426DB60E123E6B0D740E544213CD35CAB4D59D692A02A6FD97E06FE52FA32AFF05B1748141260AA618A05671E8C70C1957B2E96794EA08DEEB47834A074CF6912"));
		}


		void notAllConditionalFields_missingParameter_p()
		{
			QByteArray hexString("7F49 81FB"
								 "        06 0A 04007F00070202020203"
			        //    "        81 20 A9FB57DBA1EEA9BC3E660A909D838D726E3BF623D52620282013481D1F6E5377"
								 "        82 20 7D5A0975FC2C3057EEF67530417AFFE7FB8055C126DC5C6CE94A4B44F330B5D9"
								 "        83 20 26DC5C6CE94A4B44F330B5D9BBD77CBF958416295CF7E1CE6BCCDC18FF8C07B6"
								 "        84 41 048BD2AEB9CB7E57CB2C4B482FFC81B7AFB9DE27E1E3BD23C23A4453BD9ACE3262547EF835C3DAC4FD97F8461A14611DC9C27745132DED8E545C1D54C72F046997"
								 "        85 20 A9FB57DBA1EEA9BC3E660A909D838D718C397AA3B561A6F7901E0E82974856A7"
								 "        86 41 043347ECF96FFB4BD9B8554EFBCCFC7D0B242F1071E29B4C9C622C79E339D840AF67BEB9B912692265D9C16C62573F4579FFD4DE2DE92BAB409DD5C5D48244A9F7"
								 "        87 01 01");

			auto ecdsaPublicKey = EcdsaPublicKey::fromHex(hexString);

			QVERIFY(ecdsaPublicKey == nullptr);
		}


		void notAllConditionalFields_missingParameter_a()
		{
			QByteArray hexString("7F49 81FB"
								 "        06 0A 04007F00070202020203"
								 "        81 20 A9FB57DBA1EEA9BC3E660A909D838D726E3BF623D52620282013481D1F6E5377"
			        // "        82 20 7D5A0975FC2C3057EEF67530417AFFE7FB8055C126DC5C6CE94A4B44F330B5D9"
								 "        83 20 26DC5C6CE94A4B44F330B5D9BBD77CBF958416295CF7E1CE6BCCDC18FF8C07B6"
								 "        84 41 048BD2AEB9CB7E57CB2C4B482FFC81B7AFB9DE27E1E3BD23C23A4453BD9ACE3262547EF835C3DAC4FD97F8461A14611DC9C27745132DED8E545C1D54C72F046997"
								 "        85 20 A9FB57DBA1EEA9BC3E660A909D838D718C397AA3B561A6F7901E0E82974856A7"
								 "        86 41 043347ECF96FFB4BD9B8554EFBCCFC7D0B242F1071E29B4C9C622C79E339D840AF67BEB9B912692265D9C16C62573F4579FFD4DE2DE92BAB409DD5C5D48244A9F7"
								 "        87 01 01");

			auto ecdsaPublicKey = EcdsaPublicKey::fromHex(hexString);

			QVERIFY(ecdsaPublicKey == nullptr);
		}


		void notAllConditionalFields_missingParameter_b()
		{
			QByteArray hexString("7F49 81FB"
								 "        06 0A 04007F00070202020203"
								 "        81 20 A9FB57DBA1EEA9BC3E660A909D838D726E3BF623D52620282013481D1F6E5377"
								 "        82 20 7D5A0975FC2C3057EEF67530417AFFE7FB8055C126DC5C6CE94A4B44F330B5D9"
			        //	  "        83 20 26DC5C6CE94A4B44F330B5D9BBD77CBF958416295CF7E1CE6BCCDC18FF8C07B6"
								 "        84 41 048BD2AEB9CB7E57CB2C4B482FFC81B7AFB9DE27E1E3BD23C23A4453BD9ACE3262547EF835C3DAC4FD97F8461A14611DC9C27745132DED8E545C1D54C72F046997"
								 "        85 20 A9FB57DBA1EEA9BC3E660A909D838D718C397AA3B561A6F7901E0E82974856A7"
								 "        86 41 043347ECF96FFB4BD9B8554EFBCCFC7D0B242F1071E29B4C9C622C79E339D840AF67BEB9B912692265D9C16C62573F4579FFD4DE2DE92BAB409DD5C5D48244A9F7"
								 "        87 01 01");

			auto ecdsaPublicKey = EcdsaPublicKey::fromHex(hexString);

			QVERIFY(ecdsaPublicKey == nullptr);
		}


		void notAllConditionalFields_missingParameter_generator()
		{
			QByteArray hexString("7F49 81DA"
								 "        06 0A 04007F00070202020203"
								 "        81 20 A9FB57DBA1EEA9BC3E660A909D838D726E3BF623D52620282013481D1F6E5377"
								 "        82 20 7D5A0975FC2C3057EEF67530417AFFE7FB8055C126DC5C6CE94A4B44F330B5D9"
								 "        83 20 26DC5C6CE94A4B44F330B5D9BBD77CBF958416295CF7E1CE6BCCDC18FF8C07B6"
			        //	  "        84 41 048BD2AEB9CB7E57CB2C4B482FFC81B7AFB9DE27E1E3BD23C23A4453BD9ACE3262547EF835C3DAC4FD97F8461A14611DC9C27745132DED8E545C1D54C72F046997"
								 "        85 20 A9FB57DBA1EEA9BC3E660A909D838D718C397AA3B561A6F7901E0E82974856A7"
								 "        86 41 043347ECF96FFB4BD9B8554EFBCCFC7D0B242F1071E29B4C9C622C79E339D840AF67BEB9B912692265D9C16C62573F4579FFD4DE2DE92BAB409DD5C5D48244A9F7"
								 "        87 01 01");

			auto ecdsaPublicKey = EcdsaPublicKey::fromHex(hexString);

			QVERIFY(ecdsaPublicKey == nullptr);
		}


		void notAllConditionalFields_missingParameter_order()
		{
			QByteArray hexString("7F49 81FB"
								 "        06 0A 04007F00070202020203"
								 "        81 20 A9FB57DBA1EEA9BC3E660A909D838D726E3BF623D52620282013481D1F6E5377"
								 "        82 20 7D5A0975FC2C3057EEF67530417AFFE7FB8055C126DC5C6CE94A4B44F330B5D9"
								 "        83 20 26DC5C6CE94A4B44F330B5D9BBD77CBF958416295CF7E1CE6BCCDC18FF8C07B6"
								 "        84 41 048BD2AEB9CB7E57CB2C4B482FFC81B7AFB9DE27E1E3BD23C23A4453BD9ACE3262547EF835C3DAC4FD97F8461A14611DC9C27745132DED8E545C1D54C72F046997"
			        //	  "        85 20 A9FB57DBA1EEA9BC3E660A909D838D718C397AA3B561A6F7901E0E82974856A7"
								 "        86 41 043347ECF96FFB4BD9B8554EFBCCFC7D0B242F1071E29B4C9C622C79E339D840AF67BEB9B912692265D9C16C62573F4579FFD4DE2DE92BAB409DD5C5D48244A9F7"
								 "        87 01 01");

			auto ecdsaPublicKey = EcdsaPublicKey::fromHex(hexString);

			QVERIFY(ecdsaPublicKey == nullptr);
		}


		void allFields_except_Cofactor()
		{
			QByteArray hexString("7F49 82011A"
								 "        06 0A 04007F00070202020203"
								 "        81 20 A9FB57DBA1EEA9BC3E660A909D838D726E3BF623D52620282013481D1F6E5377"
								 "        82 20 7D5A0975FC2C3057EEF67530417AFFE7FB8055C126DC5C6CE94A4B44F330B5D9"
								 "        83 20 26DC5C6CE94A4B44F330B5D9BBD77CBF958416295CF7E1CE6BCCDC18FF8C07B6"
								 "        84 41 048BD2AEB9CB7E57CB2C4B482FFC81B7AFB9DE27E1E3BD23C23A4453BD9ACE3262547EF835C3DAC4FD97F8461A14611DC9C27745132DED8E545C1D54C72F046997"
								 "        85 20 A9FB57DBA1EEA9BC3E660A909D838D718C397AA3B561A6F7901E0E82974856A7"
								 "        86 41 043347ECF96FFB4BD9B8554EFBCCFC7D0B242F1071E29B4C9C622C79E339D840AF67BEB9B912692265D9C16C62573F4579FFD4DE2DE92BAB409DD5C5D48244A9F7"
			        //	  "        87 01 01"
								 "");

			auto ecdsaPublicKey = EcdsaPublicKey::fromHex(hexString);

			QVERIFY(ecdsaPublicKey != nullptr);
			QVERIFY(ecdsaPublicKey->isComplete());
			const auto& key = ecdsaPublicKey->createKey();
			QVERIFY(!key.isNull());

			BIGNUM* a = BN_new(), * b = BN_new(), * p = BN_new(), * cofactor = BN_new(), * order = BN_new();
			const auto guard = qScopeGuard([a, b, p, cofactor, order] {
					BN_clear_free(a);
					BN_clear_free(b);
					BN_clear_free(p);
					BN_clear_free(cofactor);
					BN_clear_free(order);
				});
			const auto generator = fetchEcParams(key, &a, &b, &p, &cofactor, &order);

			QCOMPARE(convert(p).toHex().toUpper(), QByteArray("A9FB57DBA1EEA9BC3E660A909D838D726E3BF623D52620282013481D1F6E5377"));
			QCOMPARE(convert(a).toHex().toUpper(), QByteArray("7D5A0975FC2C3057EEF67530417AFFE7FB8055C126DC5C6CE94A4B44F330B5D9"));
			QCOMPARE(convert(b).toHex().toUpper(), QByteArray("26DC5C6CE94A4B44F330B5D9BBD77CBF958416295CF7E1CE6BCCDC18FF8C07B6"));

			const auto parsedCofactor = convert(cofactor).toHex().toUpper();
			QVERIFY(parsedCofactor == QByteArray("") || parsedCofactor == QByteArray("01")); // https://github.com/openssl/openssl/commit/a6186f39802f94937a46f7a41ef0c86b6334b592

			QCOMPARE(convert(order).toHex().toUpper(), QByteArray("A9FB57DBA1EEA9BC3E660A909D838D718C397AA3B561A6F7901E0E82974856A7"));
			QCOMPARE(generator.toHex().toUpper(), QByteArray("048BD2AEB9CB7E57CB2C4B482FFC81B7AFB9DE27E1E3BD23C23A4453BD9ACE3262547EF835C3DAC4FD97F8461A14611DC9C27745132DED8E545C1D54C72F046997"));
			QCOMPARE(ecdsaPublicKey->getSecurityProtocol(), SecurityProtocol(KnownOid::ID_TA_ECDSA_SHA_256));
			QCOMPARE(ecdsaPublicKey->getOid(), Oid(KnownOid::ID_TA_ECDSA_SHA_256));
			QCOMPARE(ecdsaPublicKey->getUncompressedPublicPoint().toHex().toUpper(), QByteArray("043347ECF96FFB4BD9B8554EFBCCFC7D0B242F1071E29B4C9C622C79E339D840AF67BEB9B912692265D9C16C62573F4579FFD4DE2DE92BAB409DD5C5D48244A9F7"));
		}


		void allFields()
		{
			QByteArray hexString("7F49 82011D"
								 "        06 0A 04007F00070202020203"
								 "        81 20 A9FB57DBA1EEA9BC3E660A909D838D726E3BF623D52620282013481D1F6E5377"
								 "        82 20 7D5A0975FC2C3057EEF67530417AFFE7FB8055C126DC5C6CE94A4B44F330B5D9"
								 "        83 20 26DC5C6CE94A4B44F330B5D9BBD77CBF958416295CF7E1CE6BCCDC18FF8C07B6"
								 "        84 41 048BD2AEB9CB7E57CB2C4B482FFC81B7AFB9DE27E1E3BD23C23A4453BD9ACE3262547EF835C3DAC4FD97F8461A14611DC9C27745132DED8E545C1D54C72F046997"
								 "        85 20 A9FB57DBA1EEA9BC3E660A909D838D718C397AA3B561A6F7901E0E82974856A7"
								 "        86 41 043347ECF96FFB4BD9B8554EFBCCFC7D0B242F1071E29B4C9C622C79E339D840AF67BEB9B912692265D9C16C62573F4579FFD4DE2DE92BAB409DD5C5D48244A9F7"
								 "        87 01 01");

			auto ecdsaPublicKey = EcdsaPublicKey::fromHex(hexString);

			QVERIFY(ecdsaPublicKey != nullptr);
			QVERIFY(ecdsaPublicKey->isComplete());
			const auto& key = ecdsaPublicKey->createKey();
			QVERIFY(!key.isNull());

			BIGNUM* a = BN_new(), * b = BN_new(), * p = BN_new(), * cofactor = BN_new(), * order = BN_new();
			const auto guard = qScopeGuard([a, b, p, cofactor, order] {
					BN_clear_free(a);
					BN_clear_free(b);
					BN_clear_free(p);
					BN_clear_free(cofactor);
					BN_clear_free(order);
				});
			const auto generator = fetchEcParams(key, &a, &b, &p, &cofactor, &order);

			QCOMPARE(convert(p).toHex().toUpper(), QByteArray("A9FB57DBA1EEA9BC3E660A909D838D726E3BF623D52620282013481D1F6E5377"));
			QCOMPARE(convert(a).toHex().toUpper(), QByteArray("7D5A0975FC2C3057EEF67530417AFFE7FB8055C126DC5C6CE94A4B44F330B5D9"));
			QCOMPARE(convert(b).toHex().toUpper(), QByteArray("26DC5C6CE94A4B44F330B5D9BBD77CBF958416295CF7E1CE6BCCDC18FF8C07B6"));
			QCOMPARE(convert(cofactor).toHex().toUpper(), QByteArray("01"));
			QCOMPARE(convert(order).toHex().toUpper(), QByteArray("A9FB57DBA1EEA9BC3E660A909D838D718C397AA3B561A6F7901E0E82974856A7"));
			QCOMPARE(generator.toHex().toUpper(), QByteArray("048BD2AEB9CB7E57CB2C4B482FFC81B7AFB9DE27E1E3BD23C23A4453BD9ACE3262547EF835C3DAC4FD97F8461A14611DC9C27745132DED8E545C1D54C72F046997"));
			QCOMPARE(ecdsaPublicKey->getSecurityProtocol(), SecurityProtocol(KnownOid::ID_TA_ECDSA_SHA_256));
			QCOMPARE(ecdsaPublicKey->getOid(), Oid(KnownOid::ID_TA_ECDSA_SHA_256));
			QCOMPARE(ecdsaPublicKey->getUncompressedPublicPoint().toHex().toUpper(), QByteArray("043347ECF96FFB4BD9B8554EFBCCFC7D0B242F1071E29B4C9C622C79E339D840AF67BEB9B912692265D9C16C62573F4579FFD4DE2DE92BAB409DD5C5D48244A9F7"));
		}


};

QTEST_GUILESS_MAIN(test_EcdsaPublicKey)
#include "test_EcdsaPublicKey.moc"
