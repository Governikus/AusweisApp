/*!
 * \brief Unit tests for \ref CustomSchemeActivationContext
 *
 * \copyright Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "CustomSchemeActivationContext.h"

#include <QDesktopServices>
#include <QtTest>


using namespace governikus;


class test_CustomSchemeActivationContext
	: public QObject
{
	Q_OBJECT

	private:
		QUrl mReceivedUrl;

	private Q_SLOTS:
		void onCustomUrl(const QUrl& pUrl)
		{
			mReceivedUrl = pUrl;
		}


		void test_GetActivationUrl()
		{
			const QUrl testUrl("http://localhost:24727/eID-Client?tctokenURL=bla");
			CustomSchemeActivationContext context(testUrl, QString("ref"));
			QCOMPARE(context.getActivationURL(), testUrl);
		}


		void test_SendProcessing()
		{
			const QUrl testUrl("http://localhost:24727/eID-Client?tctokenURL=bla");
			CustomSchemeActivationContext context(testUrl, QString("ref"));
			QVERIFY(context.sendProcessing());
		}


		void test_SendActivationAlreadyActive()
		{
			const QUrl testUrl("http://localhost:24727/eID-Client?tctokenURL=bla");
			CustomSchemeActivationContext context(testUrl, QString("ref"));

			QSignalSpy spy(&context, &CustomSchemeActivationContext::fireShowUserInformation);
			QVERIFY(context.sendOperationAlreadyActive());
			QCOMPARE(spy.count(), 1);

			QVERIFY(context.sendOperationAlreadyActive());
			QCOMPARE(spy.count(), 2);
		}


		void test_SendErrorPage()
		{
			const QUrl testUrl("http://localhost:24727/eID-Client?tctokenURL=bla");
			CustomSchemeActivationContext context(testUrl, QString("ref"));
			QVERIFY(context.sendErrorPage(HTTP_STATUS_BAD_REQUEST, GlobalStatus::Code::Unknown_Error));
		}


		void test_SendRedirect()
		{
			mReceivedUrl = QUrl();
			QDesktopServices::setUrlHandler(QStringLiteral("https"), this, "onCustomUrl");

			{
				const QUrl testUrl("http://localhost:24727/eID-Client?tctokenURL=bla");
				CustomSchemeActivationContext context(testUrl, QString("ref"));

				QTest::ignoreMessage(QtDebugMsg, "Determined redirect URL QUrl(\"https://www.example.com?ResultMajor=ok\")");
				QVERIFY(context.sendRedirect(QUrl("https://www.example.com"), GlobalStatus::Code::No_Error));
			}

			QCOMPARE(mReceivedUrl, QUrl("https://www.example.com?ResultMajor=ok"));
			QDesktopServices::unsetUrlHandler(QStringLiteral("https"));
		}


};

QTEST_GUILESS_MAIN(test_CustomSchemeActivationContext)
#include "test_CustomSchemeActivationContext.moc"
