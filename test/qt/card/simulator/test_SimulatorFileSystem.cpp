/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */


#include "SimulatorFileSystem.h"

#include "FileRef.h"
#include "ResourceLoader.h"
#include "TestFileHelper.h"
#include "asn1/CVCertificate.h"

#include <QJsonDocument>
#include <QScopedPointer>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


Q_DECLARE_METATYPE(SimulatorFileSystem)


class test_SimulatorFileSystem
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase_data()
		{
			QTest::addColumn<SimulatorFileSystem>("fileSystem");

			ResourceLoader::getInstance().init();
			QTest::newRow("Default") << SimulatorFileSystem();

			const QByteArray bytes = TestFileHelper::readFile(":/card/simulatorFiles.json"_L1);
			const auto& files = QJsonDocument::fromJson(bytes).object();
			QTest::newRow("Json") << SimulatorFileSystem(files);
		}


		void initTestCase()
		{
			QJsonDocument doc = QJsonDocument::fromJson(QByteArray(R"({
	"files": [
		[
			{
				"fileId": "0101",
				"shortFileId": "01"
			}
		],
		{
			"fileId": "0101",
			"shortFileId": "01"
		}
	],
	"keys": [
		[
			{
				"id": 1
			}
		],
		{
			"id": 1
		}
	]
})"));

			QTest::ignoreMessage(QtWarningMsg, R"(Skipping file entry. Expected JSON object, got QJsonValue(array, QJsonArray([{"fileId":"0101","shortFileId":"01"}])))");
			QTest::ignoreMessage(QtWarningMsg, R"(Skipping file entry. Expected JSON object with 'fileId', 'shortFileId' and 'content', got QJsonObject({"fileId":"0101","shortFileId":"01"}))");
			QTest::ignoreMessage(QtWarningMsg, R"(Skipping key entry. Expected JSON object, got QJsonValue(array, QJsonArray([{"id":1}])))");
			QTest::ignoreMessage(QtWarningMsg, R"(Skipping key entry. Expected JSON object with 'id' and 'content', got QJsonObject({"id":1}))");
			SimulatorFileSystem fs(doc.object());
		}


		void systemFiles_data()
		{
			QTest::addColumn<QByteArray>("fileRef");
			QTest::addColumn<QByteArray>("value");

			QTest::newRow("FileRef - efDir") << FileRef::efDir().getIdentifier() << QByteArray::fromHex("61324F0F");
			QTest::newRow("FileRef - efCardAccess") << FileRef::efCardAccess().getIdentifier() << QByteArray::fromHex("3181C130");
			QTest::newRow("FileRef - efCardSecurity") << FileRef::efCardSecurity().getIdentifier() << QByteArray::fromHex("308209F6");

			QTest::newRow("Short File Identifier - efDir") << QByteArray::fromHex("1E") << QByteArray::fromHex("61324F0F");
			QTest::newRow("Short File Identifier - efCardAccess") << QByteArray::fromHex("1C") << QByteArray::fromHex("3181C130");
			QTest::newRow("Short File Identifier - efCardSecurity") << QByteArray::fromHex("1D") << QByteArray::fromHex("308209F6");

			QTest::newRow("File Identifier - efDir 1") << QByteArray::fromHex("2F00") << QByteArray::fromHex("61324F0F");
			QTest::newRow("File Identifier - efCardAccess") << QByteArray::fromHex("011C") << QByteArray::fromHex("3181C130");
			QTest::newRow("File Identifier - efCardSecurity") << QByteArray::fromHex("011D") << QByteArray::fromHex("308209F6");
		}


		void systemFiles()
		{
			QFETCH_GLOBAL(SimulatorFileSystem, fileSystem);
			QFETCH(QByteArray, fileRef);
			QFETCH(QByteArray, value);

			QCOMPARE(fileSystem.select(fileRef), StatusCode::SUCCESS);
			QCOMPARE(fileSystem.read(0, 4, false), value + QByteArray::fromHex("9000"));
		}


		void properties_data()
		{
			QTest::addColumn<QByteArray>("fileRef");
			QTest::addColumn<QByteArray>("value");

			QTest::newRow("01 - Document Type") << QByteArray::fromHex("01") << QByteArray::fromHex("610413024944");
			QTest::newRow("02 - Issuing State") << QByteArray::fromHex("02") << QByteArray::fromHex("6203130144");
			QTest::newRow("03 - Date of Expiry") << QByteArray::fromHex("03") << QByteArray::fromHex("630A12083230333430363330");
			QTest::newRow("04 - Given Names") << QByteArray::fromHex("04") << QByteArray::fromHex("64070C054552494B41");
			QTest::newRow("05 - Family Names") << QByteArray::fromHex("05") << QByteArray::fromHex("650C0C0A4D55535445524D414E4E");
			QTest::newRow("06 - Nom de Plume") << QByteArray::fromHex("06") << QByteArray::fromHex("66020C00");
			QTest::newRow("07 - Academic Title") << QByteArray::fromHex("07") << QByteArray::fromHex("67020C00");
			QTest::newRow("08 - Date of Birth") << QByteArray::fromHex("08") << QByteArray::fromHex("680A12083139383430383132");
			QTest::newRow("09 - Place of Birth") << QByteArray::fromHex("09") << QByteArray::fromHex("690AA1080C064245524C494E");
			QTest::newRow("10 - Nationality") << QByteArray::fromHex("0A") << QByteArray::fromHex("6A03130144");
			QTest::newRow("11 - Sex") << QByteArray::fromHex("0B") << QByteArray::fromHex("6B03130146");
			QTest::newRow("12 - Optional Data") << QByteArray::fromHex("0C") << QByteArray::fromHex("6C30312E302C06072A8648CE3D0101022100A9FB57DBA1EEA9BC3E660A909D838D726E3BF623D52620282013481D1F6E5377");
			QTest::newRow("13 - Birth Name") << QByteArray::fromHex("0D") << QByteArray::fromHex("6D080C064741424C4552");
			//            "14 - Written Signature"        Own test function
			QTest::newRow("15 - Date of Issuance") << QByteArray::fromHex("0F") << QByteArray::fromHex("6F0A12083230313931313031");
			//            "16 - RFU"
			QTest::newRow("17 - Place of Residence") << QByteArray::fromHex("11") << QByteArray::fromHex("712D302BAA120C10484549444553545241E1BA9E45203137AB070C054BC3964C4EAD03130144AE0713053531313437");
			QTest::newRow("18 - Municipality ID") << QByteArray::fromHex("12") << QByteArray::fromHex("7209040702760503150000");
			QTest::newRow("19 - Residence Permit I") << QByteArray::fromHex("13") << QByteArray::fromHex("7316A1140C125245534944454E4345205045524D49542031");
			QTest::newRow("20 - Residence Permit II") << QByteArray::fromHex("14") << QByteArray::fromHex("7416A1140C125245534944454E4345205045524D49542032");
			QTest::newRow("21 - Phone Number") << QByteArray::fromHex("15") << QByteArray::fromHex("7515131374656C3A2B34392D3033302D31323334353637");
			QTest::newRow("22 - Email Address") << QByteArray::fromHex("16") << QByteArray::fromHex("761516136572696B61406D75737465726D616E6E2E6465");
		}


		void properties()
		{
			QFETCH_GLOBAL(SimulatorFileSystem, fileSystem);
			QFETCH(QByteArray, fileRef);
			QFETCH(QByteArray, value);

			QCOMPARE(fileSystem.select(fileRef), StatusCode::SUCCESS);
			QCOMPARE(fileSystem.read(0, 256, false), value + QByteArray::fromHex("9000"));

			QCOMPARE(fileSystem.select(QByteArray::fromHex("01") + fileRef), StatusCode::SUCCESS);
			QCOMPARE(fileSystem.read(0, 256, false), value + QByteArray::fromHex("9000"));
		}


		void writtenSignature()
		{
			QFETCH_GLOBAL(SimulatorFileSystem, fileSystem);

			QCOMPARE(fileSystem.select(QByteArray::fromHex("0E")), StatusCode::SUCCESS);
			const QByteArray& image = fileSystem.read(0, 65536, true);

			QVERIFY(image.startsWith(QByteArray::fromHex("6E8276B6048276B2")));
			QVERIFY(image.endsWith(QByteArray::fromHex("9000")));
			QCOMPARE(image.size(), 30396);
		}


		void seclectReturnCode()
		{
			QFETCH_GLOBAL(SimulatorFileSystem, fileSystem);

			QCOMPARE(fileSystem.select(QByteArray::fromHex("01")), StatusCode::SUCCESS);
			QCOMPARE(fileSystem.select(QByteArray::fromHex("20")), StatusCode::INVALID_DATAFIELD);
		}


		void readReturnCode()
		{
			QFETCH_GLOBAL(SimulatorFileSystem, fileSystem);

			QCOMPARE(fileSystem.read(0, 256, false), QByteArray::fromHex("6A82"));
			QCOMPARE(fileSystem.select(QByteArray::fromHex("20")), StatusCode::INVALID_DATAFIELD);
			QCOMPARE(fileSystem.read(0, 256, false), QByteArray::fromHex("6A82"));

			QCOMPARE(fileSystem.select(QByteArray::fromHex("01")), StatusCode::SUCCESS);

			QCOMPARE(fileSystem.read(0, 0, false), QByteArray::fromHex("6700"));
			QCOMPARE(fileSystem.read(0, 65537, false), QByteArray::fromHex("6700"));

			QCOMPARE(fileSystem.read(-1, 256, false), QByteArray::fromHex("6B00"));
			QCOMPARE(fileSystem.read(7, 256, false), QByteArray::fromHex("6B00"));

			QCOMPARE(fileSystem.read(0, 5, false), QByteArray::fromHex("61041302499000"));
			QCOMPARE(fileSystem.read(0, 6, false), QByteArray::fromHex("6104130249449000"));
			QCOMPARE(fileSystem.read(0, 7, false), QByteArray::fromHex("6104130249446282"));
			QCOMPARE(fileSystem.read(0, 255, false), QByteArray::fromHex("6104130249446282"));
			QCOMPARE(fileSystem.read(0, 256, false), QByteArray::fromHex("6104130249449000"));
			QCOMPARE(fileSystem.read(0, 256, true), QByteArray::fromHex("6104130249446282"));
			QCOMPARE(fileSystem.read(0, 65535, true), QByteArray::fromHex("6104130249446282"));
			QCOMPARE(fileSystem.read(0, 65536, true), QByteArray::fromHex("6104130249449000"));

			QCOMPARE(fileSystem.read(2, 3, false), QByteArray::fromHex("1302499000"));
			QCOMPARE(fileSystem.read(2, 4, false), QByteArray::fromHex("130249449000"));
			QCOMPARE(fileSystem.read(2, 5, false), QByteArray::fromHex("130249446282"));
			QCOMPARE(fileSystem.read(2, 256, false), QByteArray::fromHex("130249446282"));
			QCOMPARE(fileSystem.read(2, 256, true), QByteArray::fromHex("130249446282"));
			QCOMPARE(fileSystem.read(2, 65536, true), QByteArray::fromHex("130249446282"));
			QCOMPARE(fileSystem.read(2, 65535, true), QByteArray::fromHex("130249446282"));

			QCOMPARE(fileSystem.read(6, 1, false), QByteArray::fromHex("6282"));
			QCOMPARE(fileSystem.read(6, 256, false), QByteArray::fromHex("6282"));
			QCOMPARE(fileSystem.read(6, 256, true), QByteArray::fromHex("6282"));
			QCOMPARE(fileSystem.read(6, 65536, true), QByteArray::fromHex("6282"));
		}


		void write()
		{
			QFETCH_GLOBAL(SimulatorFileSystem, fileSystem);

			QCOMPARE(fileSystem.write(0, QByteArray()), StatusCode::FILE_NOT_FOUND);
			QCOMPARE(fileSystem.write(1, QByteArray()), StatusCode::FILE_NOT_FOUND);

			QCOMPARE(fileSystem.select(QByteArray::fromHex("01")), StatusCode::SUCCESS);
			QCOMPARE(fileSystem.write(0, QByteArray()), StatusCode::SUCCESS);
			QCOMPARE(fileSystem.write(1, QByteArray()), StatusCode::ILLEGAL_OFFSET);
		}


		void password()
		{
			QFETCH_GLOBAL(SimulatorFileSystem, fileSystem);

			QCOMPARE(fileSystem.getPassword(PacePasswordId::PACE_MRZ), QByteArray::fromHex("C00DBFC8F0A9BF82FF5FBC1355C38822DE221C9B"));
			QCOMPARE(fileSystem.getPassword(PacePasswordId::PACE_CAN), QByteArray("500540"));
			QCOMPARE(fileSystem.getPassword(PacePasswordId::PACE_PIN), QByteArray("123456"));
			QCOMPARE(fileSystem.getPassword(PacePasswordId::PACE_PUK), QByteArray("9876543210"));
		}


		void trustPoint()
		{
			QFETCH_GLOBAL(SimulatorFileSystem, fileSystem);

			QCOMPARE(fileSystem.getTrustPoint()->getBody().getCertificateHolderReference(), QByteArray("DETESTeID00005"));

			const auto& cert7 = CVCertificate::fromRaw(QByteArray::fromHex(
					"7F218201B67F4E82016E5F290100420E44455445535465494430303030367F4982011D060A04007F0007020202020381"
					"20A9FB57DBA1EEA9BC3E660A909D838D726E3BF623D52620282013481D1F6E537782207D5A0975FC2C3057EEF6753041"
					"7AFFE7FB8055C126DC5C6CE94A4B44F330B5D9832026DC5C6CE94A4B44F330B5D9BBD77CBF958416295CF7E1CE6BCCDC"
					"18FF8C07B68441048BD2AEB9CB7E57CB2C4B482FFC81B7AFB9DE27E1E3BD23C23A4453BD9ACE3262547EF835C3DAC4FD"
					"97F8461A14611DC9C27745132DED8E545C1D54C72F0469978520A9FB57DBA1EEA9BC3E660A909D838D718C397AA3B561"
					"A6F7901E0E82974856A786410431D8F49A3095D324E52833E1354860FCD797F44730AA4B67486E10E6059A04B773E16F"
					"803A115788D307A7B99296D5AB5CBD658D3EA28D4771ED5A027DB5ADE28701015F200E44455445535465494430303030"
					"377F4C12060904007F0007030102025305FC0F13FFFF5F25060200010001035F24060203010001035F3740275BAD7EF2"
					"4614F99ABE983C6643BE5385D2C2B1D146DD481AC422B1605CA5A64F87D4B2B9F56BEEE34B8E6B6AF6F3423A21F00AAB"
					"F55F29C3771B06B22B3A0A"));
			QTest::ignoreMessage(QtDebugMsg, "Updated TrustPoint to \"DETESTeID00007\"");
			fileSystem.setTrustPoint(cert7);
			QCOMPARE(fileSystem.getTrustPoint(), cert7);
		}


		void verify_data()
		{
			QTest::addColumn<QByteArray>("authenticatedAuxiliaryData");
			QTest::addColumn<StatusCode>("ageVerification");
			QTest::addColumn<StatusCode>("validity");
			QTest::addColumn<StatusCode>("community");

			QTest::newRow("missing authenticatedAuxiliaryData") << QByteArray() << StatusCode::VERIFICATION_FAILED << StatusCode::VERIFICATION_FAILED << StatusCode::VERIFICATION_FAILED;
			QTest::newRow("unknown OID") << QByteArray::fromHex("6717 7315 0609 04007F000703010409 5308 3139363430383132") << StatusCode::VERIFICATION_FAILED << StatusCode::VERIFICATION_FAILED << StatusCode::VERIFICATION_FAILED;

			QTest::newRow("age - 1 day before birthday") << QByteArray::fromHex("6717 7315 0609 04007F000703010401 5308 3139383430383131") << StatusCode::VERIFICATION_FAILED << StatusCode::VERIFICATION_FAILED << StatusCode::VERIFICATION_FAILED;
			QTest::newRow("age - birthday") << QByteArray::fromHex("6717 7315 0609 04007F000703010401 5308 3139383430383132") << StatusCode::SUCCESS << StatusCode::VERIFICATION_FAILED << StatusCode::VERIFICATION_FAILED;
			QTest::newRow("age - 1 day after birthday") << QByteArray::fromHex("6717 7315 0609 04007F000703010401 5308 3139383430383133") << StatusCode::SUCCESS << StatusCode::VERIFICATION_FAILED << StatusCode::VERIFICATION_FAILED;

			QTest::newRow("validity - 1 day before expiry") << QByteArray::fromHex("6717 7315 0609 04007F000703010402 5308 3230333430363239") << StatusCode::VERIFICATION_FAILED << StatusCode::SUCCESS << StatusCode::VERIFICATION_FAILED;
			QTest::newRow("validity - expiry") << QByteArray::fromHex("6717 7315 0609 04007F000703010402 5308 3230333430363330") << StatusCode::VERIFICATION_FAILED << StatusCode::SUCCESS << StatusCode::VERIFICATION_FAILED;
			QTest::newRow("validity - 1 day after expiry") << QByteArray::fromHex("6717 7315 0609 04007F000703010402 5308 3230333430373031") << StatusCode::VERIFICATION_FAILED << StatusCode::VERIFICATION_FAILED << StatusCode::VERIFICATION_FAILED;

			QTest::newRow("community - match 1") << QByteArray::fromHex("6716 7314 0609 04007F000703010403 5307 02760503150000") << StatusCode::VERIFICATION_FAILED << StatusCode::VERIFICATION_FAILED << StatusCode::SUCCESS;
			QTest::newRow("community - match 2") << QByteArray::fromHex("6715 7313 0609 04007F000703010403 5306 027605031500") << StatusCode::VERIFICATION_FAILED << StatusCode::VERIFICATION_FAILED << StatusCode::SUCCESS;
			QTest::newRow("community - match 3") << QByteArray::fromHex("6714 7312 0609 04007F000703010403 5305 0276050315") << StatusCode::VERIFICATION_FAILED << StatusCode::VERIFICATION_FAILED << StatusCode::SUCCESS;
			QTest::newRow("community - match 4") << QByteArray::fromHex("6713 7311 0609 04007F000703010403 5304 02760503") << StatusCode::VERIFICATION_FAILED << StatusCode::VERIFICATION_FAILED << StatusCode::SUCCESS;
			QTest::newRow("community - no match 1") << QByteArray::fromHex("6716 7314 0609 04007F000703010403 5307 02760503150001") << StatusCode::VERIFICATION_FAILED << StatusCode::VERIFICATION_FAILED << StatusCode::VERIFICATION_FAILED;
			QTest::newRow("community - no match 2") << QByteArray::fromHex("6716 7314 0609 04007F000703010403 5307 03760503150000") << StatusCode::VERIFICATION_FAILED << StatusCode::VERIFICATION_FAILED << StatusCode::VERIFICATION_FAILED;
			QTest::newRow("community - to short") << QByteArray::fromHex("6715 7313 0609 04007F000703010403 5306 760503150000") << StatusCode::VERIFICATION_FAILED << StatusCode::VERIFICATION_FAILED << StatusCode::VERIFICATION_FAILED;
			QTest::newRow("community - to long 1") << QByteArray::fromHex("6717 7315 0609 04007F000703010403 5308 0002760503150000") << StatusCode::VERIFICATION_FAILED << StatusCode::VERIFICATION_FAILED << StatusCode::VERIFICATION_FAILED;
			QTest::newRow("community - to long 2") << QByteArray::fromHex("6717 7315 0609 04007F000703010403 5308 0276050315000000") << StatusCode::VERIFICATION_FAILED << StatusCode::VERIFICATION_FAILED << StatusCode::VERIFICATION_FAILED;
		}


		void verify()
		{
			QFETCH_GLOBAL(SimulatorFileSystem, fileSystem);
			QFETCH(QByteArray, authenticatedAuxiliaryData);
			QFETCH(StatusCode, ageVerification);
			QFETCH(StatusCode, validity);
			QFETCH(StatusCode, community);

			auto auxData = AuthenticatedAuxiliaryData::decode(authenticatedAuxiliaryData);

			QCOMPARE(fileSystem.verify(KnownOid::ID_DATE_OF_BIRTH, auxData), ageVerification);
			QCOMPARE(fileSystem.verify(KnownOid::ID_DATE_OF_EXPIRY, auxData), validity);
			QCOMPARE(fileSystem.verify(KnownOid::ID_MUNICIPALITY_ID, auxData), community);
		}


};

QTEST_GUILESS_MAIN(test_SimulatorFileSystem)
#include "test_SimulatorFileSystem.moc"
