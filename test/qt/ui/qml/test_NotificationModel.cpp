/*!
 * \brief Unit tests for \ref NotificationModel
 *
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

#include "NotificationModel.h"

#include "LogHandler.h"

#include <QLoggingCategory>
#include <QtTest>

using namespace governikus;

class test_NotificationModel
	: public QObject
{

	Q_OBJECT

	private:
		NotificationModel* mModel = nullptr;

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
		}


		void init()
		{
			mModel = new NotificationModel();
		}


		void cleanup()
		{
			delete mModel;
			mModel = nullptr;
		}


		void test_OnNewLogMsg()
		{
			QSignalSpy spy(mModel, &NotificationModel::fireLastTypeChanged);

			const QString msg("message");
			const QLoggingCategory develMode("developermode");

			for (int i = 0; i < 20; i++)
			{
				qCDebug(develMode).noquote() << msg;
				QTRY_VERIFY(mModel->mNotificationEntries.size() > i);
				QCOMPARE(mModel->mNotificationEntries.at(i).mText, msg);
				QCOMPARE(mModel->mNotificationEntries.at(i).mType, QLatin1String(develMode.categoryName()));
			}

			const QString newMsg("new message");
			const QLoggingCategory feedback("feedback");
			qCDebug(feedback).noquote() << newMsg;

			QTRY_VERIFY(spy.count() > 20);
			QCOMPARE(mModel->mNotificationEntries.at(20).mText, newMsg);
			QCOMPARE(mModel->mNotificationEntries.at(20).mType, QLatin1String(feedback.categoryName()));
		}


		void test_Data_data()
		{
			QTest::addColumn<int>("row");
			QTest::addColumn<int>("size");
			QTest::addColumn<int>("role");
			QTest::addColumn<QVariant>("output");

			int type = NotificationModel::UserRoles::TYPE;
			int time = NotificationModel::UserRoles::TIME;

			QTest::newRow("entriesEmpty") << 0 << 0 << 0 << QVariant();
			QTest::newRow("RowNumberEqualsSize") << 2 << 2 << type << QVariant();
			QTest::newRow("entriesFirstIndex0Type") << 4 << 5 << type << QVariant(QString("developermode"));
			QTest::newRow("entriesFirstIndex2Text") << 3 << 22 << 5 << QVariant(QString("message"));
			QTest::newRow("indexOutOfRange") << 10 << 5 << time << QVariant();
		}


		void test_Data()
		{
			QFETCH(int, row);
			QFETCH(int, size);
			QFETCH(int, role);
			QFETCH(QVariant, output);

			QSignalSpy spy(mModel, &NotificationModel::fireLastTypeChanged);
			QModelIndex index = mModel->createIndex(row, 0);

			const QString msg("message");
			const QLoggingCategory category("developermode");
			for (int i = 0; i < size; i++)
			{
				const auto oldCount = spy.count();
				qCDebug(category).noquote() << msg;
				QTRY_VERIFY(spy.count() > oldCount);
			}

			QCOMPARE(mModel->data(index, role), output);
		}


};

QTEST_GUILESS_MAIN(test_NotificationModel)
#include "test_NotificationModel.moc"
