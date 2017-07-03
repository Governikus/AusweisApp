/*!
 * \brief Unit tests for \ref MsgHandlerAuth
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "messages/MsgHandlerAuth.h"

#include "InternalActivationHandler.h"
#include "MessageDispatcher.h"

#include <QSignalSpy>
#include <QtTest>

Q_IMPORT_PLUGIN(InternalActivationHandler)

using namespace governikus;

class test_MsgHandlerAuth
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void undefined()
		{
			MessageDispatcher dispatcher;
			QByteArray msg("{\"cmd\": \"RUN_AUTH\"}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"tcTokenURL cannot be undefined\",\"msg\":\"AUTH\"}"));

		}


		void invalid()
		{
			MessageDispatcher dispatcher;
			QByteArray expected("{\"error\":\"Invalid tcTokenURL\",\"msg\":\"AUTH\"}");

			QByteArray msg = "{\"cmd\": \"RUN_AUTH\", \"tcTokenURL\": 2}";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = "{\"cmd\": \"RUN_AUTH\", \"tcTokenURL\": null}";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = "{\"cmd\": \"RUN_AUTH\", \"tcTokenURL\": true}";
			QCOMPARE(dispatcher.processCommand(msg), expected);
		}


		void failed()
		{
			MessageDispatcher dispatcher;
			QByteArray expected("{\"error\":\"Validation of tcTokenURL failed\",\"msg\":\"AUTH\"}");

			QByteArray msg("{\"cmd\": \"RUN_AUTH\", \"tcTokenURL\": \"\"}");
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = "{\"cmd\": \"RUN_AUTH\", \"tcTokenURL\": \"crap\"}";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = "{\"cmd\": \"RUN_AUTH\", \"tcTokenURL\": \"xyz://\"}";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = "{\"cmd\": \"RUN_AUTH\", \"tcTokenURL\": \"file://\"}";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = "{\"cmd\": \"RUN_AUTH\", \"tcTokenURL\": \"https://\"}";
			QCOMPARE(dispatcher.processCommand(msg), expected);
		}


		void runAuthCmd()
		{
			auto activationHandler = ActivationHandler::getInstance<InternalActivationHandler>();
			QVERIFY(activationHandler);
			QSignalSpy spy(activationHandler, &ActivationHandler::fireAuthenticationRequest);

			MessageDispatcher dispatcher;
			QByteArray msg = "{\"cmd\": \"RUN_AUTH\", \"tcTokenURL\": \"https://www.governikus.de/token?session=123abc\"}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray());

			QCOMPARE(spy.count(), 1);
			auto param = spy.takeFirst();
			ActivationContext* context = param.at(0).value<ActivationContext*>();
			QVERIFY(context);
			QCOMPARE(context->getActivationURL(), QUrl("http://localhost/?tcTokenURL=https%3A%2F%2Fwww.governikus.de%2Ftoken%3Fsession%3D123abc"));
		}


		void initAuth()
		{
			const QSharedPointer<WorkflowContext> context(new WorkflowContext());
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), QByteArray());
			QCOMPARE(dispatcher.finish(), QByteArray());

			const QSharedPointer<AuthContext> authContext(new AuthContext(new InternalActivationContext(QUrl("http://dummy"))));
			QCOMPARE(dispatcher.init(authContext), QByteArray("{\"msg\":\"AUTH\"}"));
		}


		void runEncoded()
		{
			auto activationHandler = ActivationHandler::getInstance<InternalActivationHandler>();
			QVERIFY(activationHandler);
			QSignalSpy spy(activationHandler, &ActivationHandler::fireAuthenticationRequest);

			MessageDispatcher dispatcher;
			QByteArray msg = "{\"cmd\": \"RUN_AUTH\", \"tcTokenURL\": \"https://test.governikus-eid.de/gov_autent/async?SAMLRequest=nVbZsppIGH4Vi7m0lEURsI4nBbIcV1TA7SbF0myy0wj49NMeTXIySWYyueTvf%2FuWrublUxNHnSsoyiBNJhjZJ7AOSOzUCRJvghm63GOxT68vpRlHVDbmK%2BgnO5BXoIQdVJiU48fJBKuKZJyaZVCOEzMG5RjaY41fLcdUnxhnRQpTO42wDl%2BWoIBo1DRNyioGhQaKa2ADY7ecYD6EWTnG8bqu3X50jfsOwJPMvA%2FFgZ3Gn9HHPYAXoMxQPcA6IlokSMx7x2%2F1EAX7XopAJcGlKnsgcO6tUOSzWUGQQNws28TGOjNxgjG2NaBphu3RnEX1hiQDepwzGvVIa8S6nM3ZBHBRZllWYJaU0EzgBKMIctQj2B7J6iQ1HpBjiukTLHPGOpsivQYOKNaIg58Cwjr7L1wjZrAns%2BP3%2FsVHSv%2BdUfMLj9jrT1h7wd9VeXb9qp7UIPB3ncvXF8TJWErsos0gcD7K%2BjXpuQxKfABBwlhl0Leq5J1NMBNxtH6DzPKtkWhC81l3P%2FhaiBTt14N%2BWng4RRAkTgxx5B6U8RfW0dvsSdVD%2Bl8kShGIkXT%2FGInArAD0U6fDR15aBNCPf2OoCUqSYnu2hTzwcPHvbou%2FvjjleAHaWeKmz1qn%2FMVIAie4O06nDLy%2FfuAKNfmfw79n%2B8%2BgF6XZIz%2FTGALyLt00yHxQ3HX7LrA3owq8nqchd%2FChs84FcV%2F7K2XeNSNam7c1zXHmaE01PIgXcC8dVEkwOF9cgKS6Gjw7z%2Fcm301Gy4G2U3ldYZNjokPNXknswARRE1X1eWSDZvgGGUNwzhS6g1yrGsui1cBFSMJwF8Do1rihPLyZPH%2B7haovT4eaFrDbAcB3ah6G24xiVGFr5%2BUlOxlvuORmrFaQuzIHyvE257rT0Qpe9xqlVeAYWIGhioZiWWcoRnVIadR5YbORNmUAawRruznL%2FsUN8GCjjEA7mpmLnAqrZRU5i22Sd91An2blxp2R7IY6VLcisRwhJslgyKjEleEbMO9yF4m4bQapCs8jnWblM6WpFru02fDCevopWnteylL1ZPKCf%2BD%2BQfV3oYccj6SvlxT5BcW%2Bue8HAf%2FMTQ8DPJYQZXuh7SMTD8zuydusrZJf86WxyozjgobSkWfDkMn5S3d7mFJTXKWEkXuKdWKGFnXOQrbYJwodtcXONjyt686N5T4ZCOtpO4PeJiLo0PDieXdIzt11lzLndd4KjLqkY3kqH287U2svdeN6852480LvYm4vTEUXbN0FOKMUtroeXtdRGHOncHqMtZTJcvEircycDWNFy89VumWnqsTBvT7jo60ypUYbLTkt97nbGOvGJm1wahNDk5q3Q93qNVgufWV6s66GlTnbzDVxejiq7ThcNUqs3FyfVmVCYbeOElnXzU0JL7wtIefm1Wwv4%2FQZF%2Bp0mCa6tiVO8qI2Z7kx8KKteIipPN7dToNqFzCU7s9xRStX25wkV3SVpjK1sXTeL%2FRmPuAovBL5dsaTDCFJSnwaDfNhxXXdZq%2BLZ1fGU8GxBHxJweVVONRvvnySDkyOV0P1EOf8fHAEx5vPnzPLSvnB4dJGAhzGbJAJFJnvDFGo9gBuzduaetsf4PHmOJc6APybfPkzDz6N%2Bd%2BPB3Lq85fh46PzJfbxvXn9Gw%3D%3D&RelayState=fc4b5fad-76fd-49d2-841b-b5cd39568029&SigAlg=http%3A%2F%2Fwww.w3.org%2F2001%2F04%2Fxmldsig-more%23rsa-sha256&Signature=jEVAVmY1XMMuNt2GJ%2BoaN6FaGj6ACZokOfme1OmkBiCPJyr%2B4rsg%2BrajIyMtSoUbtOoOaH63g1JP%0AqT7Wc%2Bucd%2FLcEYf7qU%2FJKN2uTMvssQblsSKuPHOWLJQUKPZHiSPA67%2F9JZhov8FXYM9uBYSs1Far%0AMRBpM49o1m1pRzwOKx3%2FcwOqc%2BT5mOWM78tNOj37mFFzwDTGuoB9iE2cZCUu1wfBbTPIvRZ2ND8Y%0AVYEO5FdR8W0sLR9LNOVjm64D%2Fl9u%2FjFJXntW4CPRPXiOiCr7TLlnFWVxXOTwywF5LTyLYQR5OjwB%0AK3cWmRPkN6OAejN%2F3Su3nWHDlGttrZuZBYzfag%3D%3D\"}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray());

			QCOMPARE(spy.count(), 1);
			auto param = spy.takeFirst();
			ActivationContext* context = param.at(0).value<ActivationContext*>();
			QVERIFY(context);
			QCOMPARE(context->getActivationURL(), QUrl("http://localhost/?tcTokenURL=https%3A%2F%2Ftest.governikus-eid.de%2Fgov_autent%2Fasync%3FSAMLRequest%3DnVbZsppIGH4Vi7m0lEURsI4nBbIcV1TA7SbF0myy0wj49NMeTXIySWYyueTvf%252FuWrublUxNHnSsoyiBNJhjZJ7AOSOzUCRJvghm63GOxT68vpRlHVDbmK%252BgnO5BXoIQdVJiU48fJBKuKZJyaZVCOEzMG5RjaY41fLcdUnxhnRQpTO42wDl%252BWoIBo1DRNyioGhQaKa2ADY7ecYD6EWTnG8bqu3X50jfsOwJPMvA%252FFgZ3Gn9HHPYAXoMxQPcA6IlokSMx7x2%252F1EAX7XopAJcGlKnsgcO6tUOSzWUGQQNws28TGOjNxgjG2NaBphu3RnEX1hiQDepwzGvVIa8S6nM3ZBHBRZllWYJaU0EzgBKMIctQj2B7J6iQ1HpBjiukTLHPGOpsivQYOKNaIg58Cwjr7L1wjZrAns%252BP3%252FsVHSv%252BdUfMLj9jrT1h7wd9VeXb9qp7UIPB3ncvXF8TJWErsos0gcD7K%252BjXpuQxKfABBwlhl0Leq5J1NMBNxtH6DzPKtkWhC81l3P%252FhaiBTt14N%252BWng4RRAkTgxx5B6U8RfW0dvsSdVD%252Bl8kShGIkXT%252FGInArAD0U6fDR15aBNCPf2OoCUqSYnu2hTzwcPHvbou%252FvjjleAHaWeKmz1qn%252FMVIAie4O06nDLy%252FfuAKNfmfw79n%252B8%252BgF6XZIz%252FTGALyLt00yHxQ3HX7LrA3owq8nqchd%252FChs84FcV%252F7K2XeNSNam7c1zXHmaE01PIgXcC8dVEkwOF9cgKS6Gjw7z%252Fcm301Gy4G2U3ldYZNjokPNXknswARRE1X1eWSDZvgGGUNwzhS6g1yrGsui1cBFSMJwF8Do1rihPLyZPH%252B7haovT4eaFrDbAcB3ah6G24xiVGFr5%252BUlOxlvuORmrFaQuzIHyvE257rT0Qpe9xqlVeAYWIGhioZiWWcoRnVIadR5YbORNmUAawRruznL%252FsUN8GCjjEA7mpmLnAqrZRU5i22Sd91An2blxp2R7IY6VLcisRwhJslgyKjEleEbMO9yF4m4bQapCs8jnWblM6WpFru02fDCevopWnteylL1ZPKCf%252BD%252BQfV3oYccj6SvlxT5BcW%252Bue8HAf%252FMTQ8DPJYQZXuh7SMTD8zuydusrZJf86WxyozjgobSkWfDkMn5S3d7mFJTXKWEkXuKdWKGFnXOQrbYJwodtcXONjyt686N5T4ZCOtpO4PeJiLo0PDieXdIzt11lzLndd4KjLqkY3kqH287U2svdeN6852480LvYm4vTEUXbN0FOKMUtroeXtdRGHOncHqMtZTJcvEircycDWNFy89VumWnqsTBvT7jo60ypUYbLTkt97nbGOvGJm1wahNDk5q3Q93qNVgufWV6s66GlTnbzDVxejiq7ThcNUqs3FyfVmVCYbeOElnXzU0JL7wtIefm1Wwv4%252FQZF%252Bp0mCa6tiVO8qI2Z7kx8KKteIipPN7dToNqFzCU7s9xRStX25wkV3SVpjK1sXTeL%252FRmPuAovBL5dsaTDCFJSnwaDfNhxXXdZq%252BLZ1fGU8GxBHxJweVVONRvvnySDkyOV0P1EOf8fHAEx5vPnzPLSvnB4dJGAhzGbJAJFJnvDFGo9gBuzduaetsf4PHmOJc6APybfPkzDz6N%252Bd%252BPB3Lq85fh46PzJfbxvXn9Gw%253D%253D%26RelayState%3Dfc4b5fad-76fd-49d2-841b-b5cd39568029%26SigAlg%3Dhttp%253A%252F%252Fwww.w3.org%252F2001%252F04%252Fxmldsig-more%2523rsa-sha256%26Signature%3DjEVAVmY1XMMuNt2GJ%252BoaN6FaGj6ACZokOfme1OmkBiCPJyr%252B4rsg%252BrajIyMtSoUbtOoOaH63g1JP%250AqT7Wc%252Bucd%252FLcEYf7qU%252FJKN2uTMvssQblsSKuPHOWLJQUKPZHiSPA67%252F9JZhov8FXYM9uBYSs1Far%250AMRBpM49o1m1pRzwOKx3%252FcwOqc%252BT5mOWM78tNOj37mFFzwDTGuoB9iE2cZCUu1wfBbTPIvRZ2ND8Y%250AVYEO5FdR8W0sLR9LNOVjm64D%252Fl9u%252FjFJXntW4CPRPXiOiCr7TLlnFWVxXOTwywF5LTyLYQR5OjwB%250AK3cWmRPkN6OAejN%252F3Su3nWHDlGttrZuZBYzfag%253D%253D"));
		}


		void badState()
		{
			const QSharedPointer<WorkflowContext> context(new WorkflowContext());
			MessageDispatcher dispatcher;
			dispatcher.init(context);
			QByteArray expected("{\"error\":\"RUN_AUTH\",\"msg\":\"BAD_STATE\"}");

			QByteArray msg("{\"cmd\": \"RUN_AUTH\", \"tcTokenURL\": \"https://correctUrl\"}");
			QCOMPARE(dispatcher.processCommand(msg), expected);
		}


		void result()
		{
			const QSharedPointer<AuthContext> context(new AuthContext(new InternalActivationContext(QUrl())));
			context->setStatus(GlobalStatus::Code::No_Error);
			MsgHandlerAuth msg(context);
			QCOMPARE(msg.toJson(), QByteArray("{\"msg\":\"AUTH\",\"result\":{\"major\":\"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\"}}"));
		}


		void resultWithUrl()
		{
			const QSharedPointer<AuthContext> context(new AuthContext(new InternalActivationContext(QUrl())));
			context->setStatus(GlobalStatus::Code::No_Error);
			context->setRefreshUrl(QUrl("http://www.governikus.de"));
			MsgHandlerAuth msg(context);
			QCOMPARE(msg.toJson(), QByteArray("{\"msg\":\"AUTH\",\"result\":{\"major\":\"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\"},\"url\":\"http://www.governikus.de\"}"));
		}


		void resultWithErrorUrl()
		{
			const QSharedPointer<TcToken> token(new TcToken(QByteArray("<?xml version=\"1.0\"?>\n"
																	   "<TCTokenType>\n"
																	   "	<ServerAddress/>\n"
																	   "	<SessionIdentifier/>\n"
																	   "	<RefreshAddress/>\n"
																	   "	<CommunicationErrorAddress>https://service.example.de/ComError?7eb39f62</CommunicationErrorAddress>\n"
																	   "	<Binding/>\n"
																	   "	<PathSecurity-Protocol/>\n"
																	   "	<PathSecurity-Parameters/>\n"
																	   "</TCTokenType>")));

			const QSharedPointer<AuthContext> context(new AuthContext(new InternalActivationContext(QUrl())));
			context->setStatus(GlobalStatus::Code::Workflow_Reader_Became_Inaccessible);
			context->setTcToken(token);
			MsgHandlerAuth msg(context);
			QCOMPARE(msg.toJson(), QByteArray("{\"msg\":\"AUTH\","
											  "\"result\":{\"description\":\"A Communication error occurred during processing.\","
											  "\"language\":\"en\","
											  "\"major\":\"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error\","
											  "\"message\":\"The selected card reader cannot be accessed anymore.\","
											  "\"minor\":\"http://www.bsi.bund.de/ecard/api/1.1/resultminor/al/common#communicationError\"},"
											  "\"url\":\"https://service.example.de/ComError?7eb39f62\"}"));
		}


};

QTEST_GUILESS_MAIN(test_MsgHandlerAuth)
#include "test_MsgHandlerAuth.moc"
