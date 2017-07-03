/*!
 * test_Template.cpp
 * \brief Unit tests for \ref Template
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "Template.h"

#include "ResourceLoader.h"

#include <QtCore>
#include <QtTest>

using namespace governikus;


class test_Template
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			ResourceLoader::getInstance().init();
		}


		void emptyTemplate()
		{
			Template tplt(QString::null);
			QCOMPARE(tplt.getContextKeys().size(), 0);
		}


		void getContextKeys()
		{
			Template tplt(QStringLiteral("${KEY_1}${KEY_2}${KEY_3}${KEY_1}${KEY_2}${KEY_3}"));
			QCOMPARE(tplt.getContextKeys().size(), 3);
			QVERIFY(tplt.getContextKeys().contains(QLatin1String("KEY_1")));
			QVERIFY(tplt.getContextKeys().contains(QLatin1String("KEY_2")));
			QVERIFY(tplt.getContextKeys().contains(QLatin1String("KEY_3")));
		}


		void setContextParam()
		{
			Template tplt(QStringLiteral("${KEY_1}${KEY_2}${KEY_3}${KEY_1}${KEY_2}${KEY_3}"));

			QVERIFY(tplt.setContextParameter(QStringLiteral("KEY_3"), QStringLiteral("VALUE_1")));
		}


		void setUnknownContextParam()
		{
			Template tplt(QStringLiteral("${KEY_1}${KEY_2}${KEY_3}${KEY_1}${KEY_2}${KEY_3}"));

			QVERIFY(!tplt.setContextParameter(QStringLiteral("KEY_4"), QStringLiteral("VALUE_1")));
		}


		void resetContextParam()
		{
			Template tplt(QStringLiteral("${KEY_1}"));

			tplt.setContextParameter(QStringLiteral("KEY_1"), QStringLiteral("la"));
			tplt.setContextParameter(QStringLiteral("KEY_1"), QStringLiteral("le"));
			QCOMPARE(tplt.render(), QLatin1String("le"));
		}


		void render()
		{
			Template tplt(QStringLiteral("${KEY_1}${KEY_2}${KEY_3} ${KEY_1}${KEY_2}${KEY_3}"));

			tplt.setContextParameter(QStringLiteral("KEY_1"), QStringLiteral("la"));
			tplt.setContextParameter(QStringLiteral("KEY_2"), QStringLiteral("le"));
			tplt.setContextParameter(QStringLiteral("KEY_3"), QStringLiteral("lu"));
			QCOMPARE(tplt.render(), QLatin1String("lalelu lalelu"));
		}


		void renderErrorPage()
		{
			QLatin1String title("test titel");
			QLatin1String msg_header("nachrichten header");
			QLatin1String msg_header_expl("nachricht");
			QLatin1String error_msg("fehler");
			QLatin1String error_msg_label("fehler label");
			QLatin1String report_header("report_header");
			QLatin1String report_link("report_link");
			QLatin1String report_button("report_button");

			Template tplt = Template::fromFile(QStringLiteral(":/html_templates/error.html"));

			tplt.setContextParameter(QLatin1String("TITLE"), title);
			tplt.setContextParameter(QLatin1String("MESSAGE_HEADER"), msg_header);
			tplt.setContextParameter(QLatin1String("MESSAGE_HEADER_EXPLANATION"), msg_header_expl);
			tplt.setContextParameter(QLatin1String("ERROR_MESSAGE_LABEL"), error_msg_label);
			tplt.setContextParameter(QLatin1String("ERROR_MESSAGE"), error_msg);
			tplt.setContextParameter(QStringLiteral("REPORT_HEADER"), report_header);
			tplt.setContextParameter(QStringLiteral("REPORT_LINK"), report_link);
			tplt.setContextParameter(QStringLiteral("REPORT_BUTTON"), report_button);

			const auto& errorPage = tplt.render();
			QCOMPARE(tplt.getContextKeys().size(), 8);
			QVERIFY(!errorPage.contains(QLatin1Char('$')));
			QVERIFY(errorPage.contains(title));
			QVERIFY(errorPage.contains(msg_header));
			QVERIFY(errorPage.contains(msg_header_expl));
			QVERIFY(errorPage.contains(error_msg));
			QVERIFY(errorPage.contains(error_msg_label));
			QVERIFY(errorPage.contains(report_header));
			QVERIFY(errorPage.contains(report_link));
			QVERIFY(errorPage.contains(report_button));
		}


		void renderAlreadyActivePage()
		{
			QLatin1String title("test titel");
			QLatin1String msg_header("nachrichten header");
			QLatin1String msg_header_expl("nachricht");
			QLatin1String content_header("inhalt header");
			QLatin1String content_link("inhalt link");
			QLatin1String content_button("inhalt button");

			Template tplt = Template::fromFile(QStringLiteral(":/html_templates/alreadyactive.html"));

			tplt.setContextParameter(QLatin1String("TITLE"), title);
			tplt.setContextParameter(QLatin1String("MESSAGE_HEADER"), msg_header);
			tplt.setContextParameter(QLatin1String("MESSAGE_HEADER_EXPLANATION"), msg_header_expl);
			tplt.setContextParameter(QLatin1String("CONTENT_HEADER"), content_header);
			tplt.setContextParameter(QLatin1String("CONTENT_LINK"), content_link);
			tplt.setContextParameter(QLatin1String("CONTENT_BUTTON"), content_button);

			const auto& errorPage = tplt.render();
			QCOMPARE(tplt.getContextKeys().size(), 6);
			QVERIFY(!errorPage.contains(QLatin1Char('$')));
			QVERIFY(errorPage.contains(title));
			QVERIFY(errorPage.contains(msg_header));
			QVERIFY(errorPage.contains(msg_header_expl));
			QVERIFY(errorPage.contains(content_header));
			QVERIFY(errorPage.contains(content_link));
			QVERIFY(errorPage.contains(content_button));
		}


};

QTEST_GUILESS_MAIN(test_Template)
#include "test_Template.moc"
