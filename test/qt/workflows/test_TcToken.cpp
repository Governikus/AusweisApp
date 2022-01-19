/*!
 * \brief Unit tests for \ref TcToken
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "TcToken.h"

#include <QtTest>

using namespace governikus;

class test_TcToken
	: public QObject
{
	Q_OBJECT

	private:
		QFile tokenXmlOk;
		QFile tokenXmlBroken;

		void checkAndOpenFile(QFile& file)
		{
			QVERIFY(file.exists());
			QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
		}

	public:
		test_TcToken()
			: tokenXmlOk(":/tctoken/ok.xml")
			, tokenXmlBroken(":/tctoken/broken.xml")
		{
			checkAndOpenFile(tokenXmlOk);
			checkAndOpenFile(tokenXmlBroken);
		}

	private Q_SLOTS:
		void parsedValues()
		{
			TcToken token(tokenXmlOk.readAll());
			QVERIFY(token.isValid());
			QCOMPARE(token.getBinding(), QString("urn:liberty:paos:2006-08"));
			QCOMPARE(token.getSessionIdentifier(), QByteArray("1A2BB129"));
			QCOMPARE(token.getServerAddress().toString(), QString("https://eid-server.example.de/entrypoint"));
			QCOMPARE(token.getRefreshAddress().toString(), QString("https://service.example.de/loggedin?7eb39f62"));
			QCOMPARE(token.getCommunicationErrorAddress().toString(), QString("https://service.example.de/ComError?7eb39f62"));
			QCOMPARE(token.getPsk(), QByteArray("4BC1A0B5"));
		}


		void parseTcTokenWithoutCommunicationErrorAddress()
		{
			TcToken token(QByteArray("<?xml version=\"1.0\"?>"
									 "<TCTokenType>"
									 "  <ServerAddress>https://eid-server.example.de/entrypoint</ServerAddress>"
									 "  <SessionIdentifier>1A2BB129</SessionIdentifier>"
									 "  <RefreshAddress>https://service.example.de/loggedin?7eb39f62</RefreshAddress>"
									 "  <Binding> urn:liberty:paos:2006-08 </Binding>"
									 "  <PathSecurity-Protocol> urn:ietf:rfc:4279 </PathSecurity-Protocol>"
									 "  <PathSecurity-Parameters>"
									 "    <PSK> 4BC1A0B5 </PSK>"
									 "  </PathSecurity-Parameters>"
									 "</TCTokenType>"));
			QVERIFY(token.isValid());
			QVERIFY(!token.getBinding().isEmpty());
			QVERIFY(!token.getSessionIdentifier().isEmpty());
			QVERIFY(!token.getServerAddress().isEmpty());
			QVERIFY(!token.getRefreshAddress().isEmpty());
			QVERIFY(token.getCommunicationErrorAddress().isEmpty());
			QVERIFY(token.usePsk());
		}


		/*
		 * This is required from test case EID_CLIENT_A2_05
		 */
		void parseTcTokenOnlyWithCommunicationErrorAddress()
		{
			TcToken token(QByteArray("<?xml version=\"1.0\"?>\n"
									 "<TCTokenType>\n"
									 "	<ServerAddress/>\n"
									 "	<SessionIdentifier/>\n"
									 "	<RefreshAddress/>\n"
									 "	<CommunicationErrorAddress>https://service.example.de/ComError?7eb39f62</CommunicationErrorAddress>\n"
									 "	<Binding/>\n"
									 "	<PathSecurity-Protocol/>\n"
									 "	<PathSecurity-Parameters/>\n"
									 "</TCTokenType>"));
			QVERIFY(!token.isValid());
			QVERIFY(token.getBinding().isEmpty());
			QVERIFY(token.getSessionIdentifier().isEmpty());
			QVERIFY(token.getServerAddress().isEmpty());
			QVERIFY(token.getRefreshAddress().isEmpty());
			QVERIFY(!token.getCommunicationErrorAddress().isEmpty());
			QVERIFY(!token.usePsk());
		}


		/*
		 * This is required from test case EID_CLIENT_A2_05
		 */
		void parseTcTokenOnlyWithCommunicationErrorAddress2()
		{
			TcToken token(QByteArray("<?xml version=\"1.0\"?>\n"
									 "<TCTokenType>\n"
									 "	<ServerAddress></ServerAddress>\n"
									 "	<SessionIdentifier></SessionIdentifier>\n"
									 "	<RefreshAddress></RefreshAddress>\n"
									 "	<CommunicationErrorAddress>https://service.example.de/ComError?7eb39f62</CommunicationErrorAddress>\n"
									 "	<Binding></Binding>\n"
									 "	<PathSecurity-Protocol></PathSecurity-Protocol>\n"
									 "	<PathSecurity-Parameters></PathSecurity-Parameters>\n"
									 "</TCTokenType>"));
			QVERIFY(!token.isValid());
			QVERIFY(token.getBinding().isEmpty());
			QVERIFY(token.getSessionIdentifier().isEmpty());
			QVERIFY(token.getServerAddress().isEmpty());
			QVERIFY(token.getRefreshAddress().isEmpty());
			QVERIFY(!token.getCommunicationErrorAddress().isEmpty());
			QVERIFY(!token.usePsk());
		}


		void parsePsk_oddNumberOfChars()
		{
			TcToken token(QByteArray("<?xml version=\"1.0\"?>"
									 "<TCTokenType>"
									 "  <ServerAddress>https://eid-server.example.de/entrypoint</ServerAddress>"
									 "  <SessionIdentifier>1A2BB129</SessionIdentifier>"
									 "  <RefreshAddress>https://service.example.de/loggedin?7eb39f62</RefreshAddress>"
									 "  <CommunicationErrorAddress>https://service.example.de/ComError?7eb39f62</CommunicationErrorAddress>"
									 "  <Binding> urn:liberty:paos:2006-08 </Binding>"
									 "  <PathSecurity-Protocol> urn:ietf:rfc:4279 </PathSecurity-Protocol>"
									 "  <PathSecurity-Parameters>"
									 "    <PSK> 4BC1A0B56 </PSK>"
									 "  </PathSecurity-Parameters>"
									 "</TCTokenType>"));
			QVERIFY(!token.isValid());
			QVERIFY(token.usePsk());
		}


		void parsePsk_nonHexChars()
		{
			TcToken token(QByteArray("<?xml version=\"1.0\"?>"
									 "<TCTokenType>"
									 "  <ServerAddress>https://eid-server.example.de/entrypoint</ServerAddress>"
									 "  <SessionIdentifier>1A2BB129</SessionIdentifier>"
									 "  <RefreshAddress>https://service.example.de/loggedin?7eb39f62</RefreshAddress>"
									 "  <CommunicationErrorAddress>https://service.example.de/ComError?7eb39f62</CommunicationErrorAddress>"
									 "  <Binding> urn:liberty:paos:2006-08 </Binding>"
									 "  <PathSecurity-Protocol> urn:ietf:rfc:4279 </PathSecurity-Protocol>"
									 "  <PathSecurity-Parameters>"
									 "    <PSK> 4BC1A0B5XX </PSK>"
									 "  </PathSecurity-Parameters>"
									 "</TCTokenType>"));
			QVERIFY(!token.isValid());
			QVERIFY(token.usePsk());
		}


		void parseNoPskButPathSecurityProtocol()
		{
			TcToken token(QByteArray("<?xml version=\"1.0\"?>"
									 "<TCTokenType>"
									 "  <ServerAddress>https://eid-server.example.de/entrypoint</ServerAddress>"
									 "  <SessionIdentifier>1A2BB129</SessionIdentifier>"
									 "  <RefreshAddress>https://service.example.de/loggedin?7eb39f62</RefreshAddress>"
									 "  <CommunicationErrorAddress>https://service.example.de/ComError?7eb39f62</CommunicationErrorAddress>"
									 "  <Binding> urn:liberty:paos:2006-08 </Binding>"
									 "  <PathSecurity-Protocol> urn:ietf:rfc:4279 </PathSecurity-Protocol>"
									 "  <PathSecurity-Parameters>"
									 "  </PathSecurity-Parameters>"
									 "</TCTokenType>"));
			QVERIFY(token.isValid());
			QVERIFY(token.isSchemaConform());
		}


		void parseNoPathSecurityParametersButPathSecurityProtocol()
		{
			TcToken token(QByteArray("<?xml version=\"1.0\"?>"
									 "<TCTokenType>"
									 "  <ServerAddress>https://eid-server.example.de/entrypoint</ServerAddress>"
									 "  <SessionIdentifier>1A2BB129</SessionIdentifier>"
									 "  <RefreshAddress>https://service.example.de/loggedin?7eb39f62</RefreshAddress>"
									 "  <CommunicationErrorAddress>https://service.example.de/ComError?7eb39f62</CommunicationErrorAddress>"
									 "  <Binding> urn:liberty:paos:2006-08 </Binding>"
									 "  <PathSecurity-Protocol> urn:ietf:rfc:4279 </PathSecurity-Protocol>"
									 "</TCTokenType>"));
			QVERIFY(token.isValid());
			QVERIFY(token.isSchemaConform());
		}


		void tryToParseCrap()
		{
			TcToken token(tokenXmlBroken.readAll());
			QVERIFY(!token.isValid());
			QVERIFY(token.getBinding().isNull());
			QVERIFY(token.getSessionIdentifier().isNull());
			QVERIFY(token.getServerAddress().isEmpty());
			QVERIFY(token.getRefreshAddress().isEmpty());
			QVERIFY(token.getCommunicationErrorAddress().isEmpty());
			QVERIFY(!token.usePsk());
		}


		void tryToParseClosedFile()
		{
			tokenXmlOk.close();
			TcToken token(tokenXmlOk.readAll());
			QVERIFY(!token.isValid());
		}


		void test_IsValid_data()
		{
			QTest::addColumn<QByteArray>("data");
			QTest::addColumn<bool>("valid");

			QTest::newRow("noSchemaConform") << QByteArray("<?xml version=\"1.0\"?>"
														   "<TCTokenType>"
														   "  <ServerAddress>https://eid-server.example.de/entrypoint</ServerAddress>"
														   "  <SessionIdentifier>1A2BB129</SessionIdentifier>"
														   "  <RefreshAddress>https://service.example.de/loggedin?7eb39f62</RefreshAddress>"
														   "  <CommunicationErrorAddress>https://service.example.de/ComError?7eb39f62</CommunicationErrorAddress>"
														   "  <Binding> </Binding>"
														   "  <PathSecurity-Protocol> urn:ietf:rfc:4279 </PathSecurity-Protocol>"
														   "  <PathSecurity-Parameters>"
														   "    <PSK> 4BC1A0B5 </PSK>"
														   "  </PathSecurity-Parameters>"
														   "</TCTokenType>") << false;

			QTest::newRow("invalidBinding") << QByteArray("<?xml version=\"1.0\"?>"
														  "<TCTokenType>"
														  "  <ServerAddress>https://eid-server.example.de/entrypoint</ServerAddress>"
														  "  <SessionIdentifier>1A2BB129</SessionIdentifier>"
														  "  <RefreshAddress>https://service.example.de/loggedin?7eb39f62</RefreshAddress>"
														  "  <CommunicationErrorAddress>https://service.example.de/ComError?7eb39f62</CommunicationErrorAddress>"
														  "  <Binding>binding</Binding>"
														  "  <PathSecurity-Protocol> urn:ietf:rfc:4279 </PathSecurity-Protocol>"
														  "  <PathSecurity-Parameters>"
														  "    <PSK> 4BC1A0B5 </PSK>"
														  "  </PathSecurity-Parameters>"
														  "</TCTokenType>") << false;

			QTest::newRow("invalidSecurityProtocol") << QByteArray("<?xml version=\"1.0\"?>"
																   "<TCTokenType>"
																   "  <ServerAddress>https://eid-server.example.de/entrypoint</ServerAddress>"
																   "  <SessionIdentifier>1A2BB129</SessionIdentifier>"
																   "  <RefreshAddress>https://service.example.de/loggedin?7eb39f62</RefreshAddress>"
																   "  <CommunicationErrorAddress>https://service.example.de/ComError?7eb39f62</CommunicationErrorAddress>"
																   "  <Binding> urn:liberty:paos:2006-08 </Binding>"
																   "  <PathSecurity-Protocol>securityProtocol</PathSecurity-Protocol>"
																   "  <PathSecurity-Parameters>"
																   "    <PSK> 4BC1A0B5 </PSK>"
																   "  </PathSecurity-Parameters>"
																   "</TCTokenType>") << false;

			QTest::newRow("invalidServerAdress") << QByteArray("<?xml version=\"1.0\"?>"
															   "<TCTokenType>"
															   "  <ServerAddress>eid-server.example.de/entrypoint</ServerAddress>"
															   "  <SessionIdentifier>1A2BB129</SessionIdentifier>"
															   "  <RefreshAddress>https://service.example.de/loggedin?7eb39f62</RefreshAddress>"
															   "  <CommunicationErrorAddress>https://service.example.de/ComError?7eb39f62</CommunicationErrorAddress>"
															   "  <Binding> urn:liberty:paos:2006-08 </Binding>"
															   "  <PathSecurity-Protocol> urn:ietf:rfc:4279 </PathSecurity-Protocol>"
															   "  <PathSecurity-Parameters>"
															   "    <PSK> 4BC1A0B5 </PSK>"
															   "  </PathSecurity-Parameters>"
															   "</TCTokenType>") << false;

			QTest::newRow("invalidRefreshAdress") << QByteArray("<?xml version=\"1.0\"?>"
																"<TCTokenType>"
																"  <ServerAddress>https://eid-server.example.de/entrypoint</ServerAddress>"
																"  <SessionIdentifier>1A2BB129</SessionIdentifier>"
																"  <RefreshAddress>service.example.de/loggedin?7eb39f62</RefreshAddress>"
																"  <CommunicationErrorAddress>https://service.example.de/ComError?7eb39f62</CommunicationErrorAddress>"
																"  <Binding> urn:liberty:paos:2006-08 </Binding>"
																"  <PathSecurity-Protocol> urn:ietf:rfc:4279 </PathSecurity-Protocol>"
																"  <PathSecurity-Parameters>"
																"    <PSK> 4BC1A0B5 </PSK>"
																"  </PathSecurity-Parameters>"
																"</TCTokenType>") << false;

			QTest::newRow("invalidRefreshAdress") << QByteArray("<?xml version=\"1.0\"?>"
																"<TCTokenType>"
																"  <ServerAddress>https://eid-server.example.de/entrypoint</ServerAddress>"
																"  <SessionIdentifier>1A2BB129</SessionIdentifier>"
																"  <RefreshAddress>https://service.example.de/loggedin?7eb39f62</RefreshAddress>"
																"  <CommunicationErrorAddress>https://service.example.de/ComError?7eb39f62</CommunicationErrorAddress>"
																"  <Binding> urn:liberty:paos:2006-08 </Binding>"
																"  <PathSecurity-Protocol> urn:ietf:rfc:4279 </PathSecurity-Protocol>"
																"  <PathSecurity-Parameters>"
																"    <PSK> 4BC1A0B56 </PSK>"
																"  </PathSecurity-Parameters>"
																"</TCTokenType>") << false;

			QTest::newRow("valid") << QByteArray("<?xml version=\"1.0\"?>"
												 "<TCTokenType>"
												 "  <ServerAddress>https://eid-server.example.de/entrypoint</ServerAddress>"
												 "  <SessionIdentifier>1A2BB129</SessionIdentifier>"
												 "  <RefreshAddress>https://service.example.de/loggedin?7eb39f62</RefreshAddress>"
												 "  <CommunicationErrorAddress>https://service.example.de/ComError?7eb39f62</CommunicationErrorAddress>"
												 "  <Binding> urn:liberty:paos:2006-08 </Binding>"
												 "  <PathSecurity-Protocol> urn:ietf:rfc:4279 </PathSecurity-Protocol>"
												 "  <PathSecurity-Parameters>"
												 "    <PSK> 4BC1A0B5 </PSK>"
												 "  </PathSecurity-Parameters>"
												 "</TCTokenType>") << true;
		}


		void test_IsValid()
		{
			QFETCH(QByteArray, data);
			QFETCH(bool, valid);

			TcToken token(data);
			QCOMPARE(token.isValid(), valid);
		}


		void test_ValuesAreSchemaConform()
		{
			TcToken token(QByteArray("<?xml version=\"1.0\"?>"
									 "<TCTokenType>"
									 "  <ServerAddress>https://eid-server.example.de/entrypoint</ServerAddress>"
									 "  <SessionIdentifier>1A2BB129</SessionIdentifier>"
									 "  <RefreshAddress>https://service.example.de/loggedin?7eb39f62</RefreshAddress>"
									 "  <CommunicationErrorAddress>https://service.example.de/ComError?7eb39f62</CommunicationErrorAddress>"
									 "  <Binding> urn:liberty:paos:2006-08 </Binding>"
									 "  <PathSecurity-Protocol> urn:ietf:rfc:4279 </PathSecurity-Protocol>"
									 "  <PathSecurity-Parameters>"
									 "    <PSK> 4BC1A0B5 </PSK>"
									 "  </PathSecurity-Parameters>"
									 "</TCTokenType>"));
			const QString binding = QStringLiteral("urn:liberty:paos:2006-08");
			const QString pathProtocol = QStringLiteral("urn:ietf:rfc:4279");
			const QByteArray psk("4BC1A0B5");
			const QByteArray identifier("1A2BB129");
			const QString serverAdress = QStringLiteral("https://eid-server.example.de/entrypoint");
			const QString errorAdress = QStringLiteral("https://service.example.de/ComError?7eb39f62");
			const QString refreshAdress = QStringLiteral("https://service.example.de/loggedin?7eb39f62");

			QTest::ignoreMessage(QtCriticalMsg, "Binding is no valid anyUri: \"\"");
			QVERIFY(!token.valuesAreSchemaConform(QString(), pathProtocol, psk,
					identifier, serverAdress, errorAdress, refreshAdress));
			QTest::ignoreMessage(QtCriticalMsg, "Binding is no valid anyUri: \"://://\"");
			QVERIFY(!token.valuesAreSchemaConform(QString("://://"), pathProtocol, psk,
					identifier, serverAdress, errorAdress, refreshAdress));

			QTest::ignoreMessage(QtCriticalMsg, "PathSecurity-Protocol is no valid URI: \"\"");
			QVERIFY(token.valuesAreSchemaConform(binding, QString(""), psk,
					identifier, serverAdress, errorAdress, refreshAdress));

			QTest::ignoreMessage(QtWarningMsg, "PSK is null");
			QVERIFY(token.valuesAreSchemaConform(binding, pathProtocol, QByteArray(),
					identifier, serverAdress, errorAdress, refreshAdress));

			QTest::ignoreMessage(QtWarningMsg, "SessionIdentifier is null");
			QVERIFY(token.valuesAreSchemaConform(binding, pathProtocol, psk,
					QByteArray(), serverAdress, errorAdress, refreshAdress));

			QTest::ignoreMessage(QtCriticalMsg, "ServerAddress no valid anyUri: \"\"");
			QVERIFY(!token.valuesAreSchemaConform(binding, pathProtocol, psk,
					identifier, QString(), errorAdress, refreshAdress));
			QTest::ignoreMessage(QtCriticalMsg, "ServerAddress no valid anyUri: \"://://\"");
			QVERIFY(!token.valuesAreSchemaConform(binding, pathProtocol, psk,
					identifier, QString("://://"), errorAdress, refreshAdress));

			QTest::ignoreMessage(QtCriticalMsg, "RefreshAddress no valid anyUri: \"\"");
			QVERIFY(!token.valuesAreSchemaConform(binding, pathProtocol, psk,
					identifier, serverAdress, errorAdress, QString()));
			QTest::ignoreMessage(QtCriticalMsg, "RefreshAddress no valid anyUri: \"://://\"");
			QVERIFY(!token.valuesAreSchemaConform(binding, pathProtocol, psk,
					identifier, serverAdress, errorAdress, QString("://://")));

			QTest::ignoreMessage(QtCriticalMsg, "CommunicationErrorAddress no  valid anyUri: \"://://\"");
			QVERIFY(!token.valuesAreSchemaConform(binding, pathProtocol, psk,
					identifier, serverAdress, QString("://://"), refreshAdress));
		}


};

QTEST_GUILESS_MAIN(test_TcToken)
#include "test_TcToken.moc"
