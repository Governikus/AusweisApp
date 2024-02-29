/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref Template
 */

#include "Template.h"

#include "ResourceLoader.h"

#include <QtCore>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_Template
	: public QObject
{
	Q_OBJECT

	const QString KEY_1 = QStringLiteral("KEY_1");
	const QString KEY_2 = QStringLiteral("KEY_2");
	const QString KEY_3 = QStringLiteral("KEY_3");

	private Q_SLOTS:
		void initTestCase()
		{
			ResourceLoader::getInstance().init();
		}


		void emptyTemplate()
		{
			Template tplt(""_L1);
			QCOMPARE(tplt.getContextKeys().size(), 0);
		}


		void getContextKeys()
		{
			Template tplt(QStringLiteral("${KEY_1}${KEY_2}${KEY_3}${KEY_1}${KEY_2}${KEY_3}"));
			QCOMPARE(tplt.getContextKeys().size(), 3);
			QVERIFY(tplt.getContextKeys().contains(KEY_1));
			QVERIFY(tplt.getContextKeys().contains(KEY_2));
			QVERIFY(tplt.getContextKeys().contains(KEY_3));
		}


		void setContextParam()
		{
			Template tplt(QStringLiteral("${KEY_1}${KEY_2}${KEY_3}${KEY_1}${KEY_2}${KEY_3}"));

			QVERIFY(tplt.setContextParameter(KEY_3, QStringLiteral("VALUE_1")));
		}


		void setUnknownContextParam()
		{
			Template tplt(QStringLiteral("${KEY_1}${KEY_2}${KEY_3}${KEY_1}${KEY_2}${KEY_3}"));

			QVERIFY(!tplt.setContextParameter(QStringLiteral("KEY_4"), QStringLiteral("VALUE_1")));
		}


		void resetContextParam()
		{
			Template tplt(QStringLiteral("${KEY_1}"));

			tplt.setContextParameter(KEY_1, QStringLiteral("la"));
			tplt.setContextParameter(KEY_1, QStringLiteral("le"));
			QCOMPARE(tplt.render(), QLatin1String("le"));
		}


		void render()
		{
			Template tplt(QStringLiteral("${KEY_1}${KEY_2}${KEY_3} ${KEY_1}${KEY_2}${KEY_3}"));

			tplt.setContextParameter(KEY_1, QStringLiteral("la"));
			tplt.setContextParameter(KEY_2, QStringLiteral("le"));
			tplt.setContextParameter(KEY_3, QStringLiteral("lu"));
			QCOMPARE(tplt.render(), QLatin1String("lalelu lalelu"));
		}


		void renderErrorPage()
		{
			QString title("test titel"_L1);
			QString application_link("application_link"_L1);
			QString msg_header("nachrichten header"_L1);
			QString msg_header_expl("nachricht"_L1);
			QString error_msg("fehler"_L1);
			QString error_msg_label("fehler label"_L1);
			QString report_header("report_header"_L1);
			QString report_link("report_link"_L1);
			QString report_button("report_button"_L1);

			Template tplt = Template::fromFile(QStringLiteral(":/template.html"));

			tplt.setContextParameter(QStringLiteral("TITLE"), title);
			tplt.setContextParameter(QStringLiteral("APPLICATION_LINK"), application_link);
			tplt.setContextParameter(QStringLiteral("MESSAGE_HEADER"), msg_header);
			tplt.setContextParameter(QStringLiteral("MESSAGE_HEADER_EXPLANATION"), msg_header_expl);
			tplt.setContextParameter(QStringLiteral("MESSAGE_SUBHEADER_LABEL"), error_msg_label);
			tplt.setContextParameter(QStringLiteral("MESSAGE_SUBHEADER"), error_msg);
			tplt.setContextParameter(QStringLiteral("CONTENT_HEADER"), report_header);
			tplt.setContextParameter(QStringLiteral("CONTENT_LINK"), report_link);
			tplt.setContextParameter(QStringLiteral("CONTENT_BUTTON"), report_button);

			const auto& errorPage = tplt.render();
			QCOMPARE(tplt.getContextKeys().size(), 9);
			QVERIFY(!errorPage.contains('$'_L1));
			QVERIFY(errorPage.contains(title));
			QVERIFY(errorPage.contains(application_link));
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
			QString title("test titel"_L1);
			QString application_link("application_link"_L1);
			QString msg_header("nachrichten header"_L1);
			QString msg_header_expl("nachricht"_L1);
			QString content_header("inhalt header"_L1);
			QString content_link("inhalt link"_L1);
			QString content_button("inhalt button"_L1);

			Template tplt = Template::fromFile(QStringLiteral(":/template.html"));

			tplt.setContextParameter(QStringLiteral("TITLE"), title);
			tplt.setContextParameter(QStringLiteral("APPLICATION_LINK"), application_link);
			tplt.setContextParameter(QStringLiteral("MESSAGE_HEADER"), msg_header);
			tplt.setContextParameter(QStringLiteral("MESSAGE_HEADER_EXPLANATION"), msg_header_expl);
			tplt.setContextParameter(QStringLiteral("CONTENT_HEADER"), content_header);
			tplt.setContextParameter(QStringLiteral("CONTENT_LINK"), content_link);
			tplt.setContextParameter(QStringLiteral("CONTENT_BUTTON"), content_button);

			QTest::ignoreMessage(QtDebugMsg, "No parameter specified, replace with empty string: \"MESSAGE_SUBHEADER\"");
			QTest::ignoreMessage(QtDebugMsg, "No parameter specified, replace with empty string: \"MESSAGE_SUBHEADER_LABEL\"");
			const auto& errorPage = tplt.render();
			QCOMPARE(tplt.getContextKeys().size(), 9);
			QVERIFY(!errorPage.contains('$'_L1));
			QVERIFY(errorPage.contains(title));
			QVERIFY(errorPage.contains(application_link));
			QVERIFY(errorPage.contains(msg_header));
			QVERIFY(errorPage.contains(msg_header_expl));
			QVERIFY(errorPage.contains(content_header));
			QVERIFY(errorPage.contains(content_link));
			QVERIFY(errorPage.contains(content_button));
		}


};

QTEST_GUILESS_MAIN(test_Template)
#include "test_Template.moc"
