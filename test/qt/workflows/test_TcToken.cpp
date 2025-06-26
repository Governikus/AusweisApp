/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "TcToken.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

Q_DECLARE_METATYPE(QtMsgType)
Q_DECLARE_LOGGING_CATEGORY(secure)

class test_TcToken
	: public QObject
{
	Q_OBJECT

	private:
		const QByteArray mValidToken = QByteArray("<?xml version=\"1.0\"?>"
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
												  "</TCTokenType>");

	private Q_SLOTS:
		void parsedValues()
		{
			TcToken token(mValidToken);
			QVERIFY(token.isValid());
			QCOMPARE(token.getBinding(), "urn:liberty:paos:2006-08"_L1);
			QCOMPARE(token.getSessionIdentifier(), QByteArray("1A2BB129"));
			QCOMPARE(token.getServerAddress().toString(), "https://eid-server.example.de/entrypoint"_L1);
			QCOMPARE(token.getRefreshAddress().toString(), "https://service.example.de/loggedin?7eb39f62"_L1);
			QCOMPARE(token.getCommunicationErrorAddress().toString(), "https://service.example.de/ComError?7eb39f62"_L1);
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
			TcToken token(QByteArray("<?xml version=\"1.0\"?>"
									 "<TCTokenType>"
									 "  <ServerAddress>https://eid-server.example.de/entrypoint</ServerAddress>"
									 "  <SessionIdentifier>1meters>"
									 "  <PSK> 4BC1A0B5 </PSK>"
									 "  </PathSecurity-Parameters>"
									 "</TCTokenType>"));
			QVERIFY(!token.isValid());
			QVERIFY(token.getBinding().isNull());
			QVERIFY(token.getSessionIdentifier().isNull());
			QVERIFY(token.getServerAddress().isEmpty());
			QVERIFY(token.getRefreshAddress().isEmpty());
			QVERIFY(token.getCommunicationErrorAddress().isEmpty());
			QVERIFY(!token.usePsk());
		}


		void tryToParseEmptyData()
		{
			const QByteArray data;
			TcToken token(data);
			QVERIFY(!token.isValid());
		}


		void isValid_data()
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

			QTest::newRow("invalidServerAddress") << QByteArray("<?xml version=\"1.0\"?>"
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

			QTest::newRow("invalidRefreshAddress") << QByteArray("<?xml version=\"1.0\"?>"
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

			QTest::newRow("invalidRefreshAddress") << QByteArray("<?xml version=\"1.0\"?>"
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


		void isValid()
		{
			QFETCH(QByteArray, data);
			QFETCH(bool, valid);

			TcToken token(data);
			QCOMPARE(token.isValid(), valid);
		}


		void valuesAreSchemaConform_data()
		{
			QTest::addColumn<QString>("binding");
			QTest::addColumn<QString>("pathProtocol");
			QTest::addColumn<QByteArray>("psk");
			QTest::addColumn<QByteArray>("identifier");
			QTest::addColumn<QString>("serverAddress");
			QTest::addColumn<QString>("errorAddress");
			QTest::addColumn<QString>("refreshAddress");

			QTest::addColumn<QtMsgType>("messageType");
			QTest::addColumn<bool>("valuesAreSchemaConform");

			const QString binding = QStringLiteral("urn:liberty:paos:2006-08");
			const QString pathProtocol = QStringLiteral("urn:ietf:rfc:4279");
			const QByteArray psk("4BC1A0B5");
			const QByteArray identifier("1A2BB129");
			const QString serverAddress = QStringLiteral("https://eid-server.example.de/entrypoint");
			const QString errorAddress = QStringLiteral("https://service.example.de/ComError?7eb39f62");
			const QString refreshAddress = QStringLiteral("https://service.example.de/loggedin?7eb39f62");

			QTest::newRow("Binding is no valid anyUri: \"\"")
				<< QString() << pathProtocol << psk << identifier
				<< serverAddress << errorAddress << refreshAddress
				<< QtCriticalMsg << false;

			QTest::newRow("Binding is no valid anyUri: \"://://\"")
				<< u"://://"_s << pathProtocol << psk << identifier
				<< serverAddress << errorAddress << refreshAddress
				<< QtCriticalMsg << false;

			QTest::newRow("PathSecurity-Protocol is no valid URI: \"\"")
				<< binding << u""_s << psk << identifier
				<< serverAddress << errorAddress << refreshAddress
				<< QtCriticalMsg << true;

			QTest::newRow("PSK is null")
				<< binding << pathProtocol << QByteArray() << identifier
				<< serverAddress << errorAddress << refreshAddress
				<< QtWarningMsg << true;

			QTest::newRow("SessionIdentifier is null")
				<< binding << pathProtocol << psk << QByteArray()
				<< serverAddress << errorAddress << refreshAddress
				<< QtWarningMsg << true;

			QTest::newRow("ServerAddress no valid anyUri: \"\"")
				<< binding << pathProtocol << psk << identifier
				<< QString() << errorAddress << refreshAddress
				<< QtCriticalMsg << false;

			QTest::newRow("ServerAddress no valid anyUri: \"://://\"")
				<< binding << pathProtocol << psk << identifier
				<< u"://://"_s << errorAddress << refreshAddress
				<< QtCriticalMsg << false;

			QTest::newRow("CommunicationErrorAddress no valid anyUri: \"://://\"")
				<< binding << pathProtocol << psk << identifier
				<< serverAddress << u"://://"_s << refreshAddress
				<< QtCriticalMsg << false;

			QTest::newRow("RefreshAddress no valid anyUri: \"\"")
				<< binding << pathProtocol << psk << identifier
				<< serverAddress << errorAddress << QString()
				<< QtCriticalMsg << false;

			QTest::newRow("RefreshAddress no valid anyUri: \"://://\"")
				<< binding << pathProtocol << psk << identifier
				<< serverAddress << errorAddress << u"://://"_s
				<< QtCriticalMsg << false;
		}


		void valuesAreSchemaConform()
		{
			QFETCH(QString, binding);
			QFETCH(QString, pathProtocol);
			QFETCH(QByteArray, psk);
			QFETCH(QByteArray, identifier);
			QFETCH(QString, serverAddress);
			QFETCH(QString, errorAddress);
			QFETCH(QString, refreshAddress);

			QFETCH(QtMsgType, messageType);
			QFETCH(bool, valuesAreSchemaConform);

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

			QTest::ignoreMessage(messageType, QTest::currentDataTag());
			QCOMPARE(token.valuesAreSchemaConform(binding, pathProtocol, psk, identifier, serverAddress, errorAddress, refreshAddress), valuesAreSchemaConform);
		}


		void pskOutput()
		{
			QTest::ignoreMessage(QtDebugMsg, "\"PSK\" = \"(...)\"");
			TcToken token(mValidToken);
			QVERIFY(token.isValid());
			QCOMPARE(token.getPsk(), "4BC1A0B5");

			QLoggingCategory::setFilterRules(QStringLiteral("secure.debug=true"));
			QTest::ignoreMessage(QtDebugMsg, "\"PSK\" = \"4BC1A0B5\"");
			TcToken token2(mValidToken);
			QVERIFY(token2.isValid());
			QCOMPARE(token.getPsk(), "4BC1A0B5");
		}


};

QTEST_GUILESS_MAIN(test_TcToken)
#include "test_TcToken.moc"
