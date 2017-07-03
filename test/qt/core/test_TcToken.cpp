/*!
 * \brief Unit tests for \ref TcToken
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
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

			token.clearPsk();
			QVERIFY(token.getPsk().isNull());
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
			QVERIFY(!token.getPsk().isNull());
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
			QVERIFY(token.getPsk().isNull());
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
			QVERIFY(token.getPsk().isNull());
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
			QVERIFY(!token.getPsk().isNull());
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
			QVERIFY(!token.getPsk().isNull());
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
			QVERIFY(token.getPsk().isNull());

			token.clearPsk();
			QVERIFY(token.getPsk().isNull());
		}


		void tryToParseClosedFile()
		{
			tokenXmlOk.close();
			TcToken token(tokenXmlOk.readAll());
			QVERIFY(!token.isValid());
		}


};

QTEST_GUILESS_MAIN(test_TcToken)
#include "test_TcToken.moc"
