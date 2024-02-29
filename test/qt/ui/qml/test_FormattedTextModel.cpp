/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref FormattedTextModel
 */

#include "FormattedTextModel.h"

#include <QtTest>

using namespace governikus;

using PairList = QList<QPair<QString, FormattedTextModel::LineType>>;

class test_FormattedTextModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void init()
		{
		}


		void test_LineType_data()
		{
			QTest::addColumn<QString>("line");
			QTest::addColumn<FormattedTextModel::LineType>("type");

			QTest::newRow("Empty") << "   " << FormattedTextModel::LineType::EMPTY;

			QTest::newRow("Header") << "***" << FormattedTextModel::LineType::HEADER;
			QTest::newRow("HeaderWithSpaces") << "   ***   " << FormattedTextModel::LineType::HEADER;

			QTest::newRow("Section") << "===" << FormattedTextModel::LineType::SECTION;
			QTest::newRow("SectionWithSpaces") << "   ===   " << FormattedTextModel::LineType::SECTION;

			QTest::newRow("SubSection") << "---" << FormattedTextModel::LineType::SUBSECTION;
			QTest::newRow("SubSectionWithSpaces") << "   ---   " << FormattedTextModel::LineType::SUBSECTION;

			QTest::newRow("ListItem") << "* FOO" << FormattedTextModel::LineType::LISTITEM;
			QTest::newRow("ListItemPrecedingSpaces") << "   * FOO" << FormattedTextModel::LineType::LISTITEM;
			QTest::newRow("ListItemNoSeparatingSpace") << "*FOO" << FormattedTextModel::LineType::LISTITEM;
		}


		void test_LineType()
		{
			QFETCH(QString, line);
			QFETCH(FormattedTextModel::LineType, type);

			QCOMPARE(FormattedTextModel::lineType(line), type);
		}


		void test_isFormattingLine_data()
		{
			QTest::addColumn<FormattedTextModel::LineType>("type");
			QTest::addColumn<bool>("isFormatting");

			QTest::newRow("Empty") << FormattedTextModel::LineType::EMPTY << false;
			QTest::newRow("Header") << FormattedTextModel::LineType::HEADER << true;
			QTest::newRow("Section") << FormattedTextModel::LineType::SECTION << true;
			QTest::newRow("Subsection") << FormattedTextModel::LineType::SUBSECTION << true;
			QTest::newRow("Regulartext") << FormattedTextModel::LineType::REGULARTEXT << false;
			QTest::newRow("Listitem") << FormattedTextModel::LineType::LISTITEM << false;
		}


		void test_isFormattingLine()
		{
			QFETCH(FormattedTextModel::LineType, type);
			QFETCH(bool, isFormatting);

			QCOMPARE(FormattedTextModel::isFormattingLine(type), isFormatting);
		}


		void test_stripFormattingCharacters_data()
		{
			QTest::addColumn<QString>("line");
			QTest::addColumn<FormattedTextModel::LineType>("type");
			QTest::addColumn<QString>("strippedLine");

			QTest::addRow("ListItem") << "* FOO" << FormattedTextModel::LineType::LISTITEM << "FOO";
			QTest::addRow("ListItemWithSpace") << "*  FOO" << FormattedTextModel::LineType::LISTITEM << " FOO";
		}


		void test_stripFormattingCharacters()
		{
			QFETCH(QString, line);
			QFETCH(FormattedTextModel::LineType, type);
			QFETCH(QString, strippedLine);

			QCOMPARE(FormattedTextModel::stripFormattingCharacters(line, type), strippedLine);
		}


		void test_replaceControlCharactersWithHtml_data()
		{
			QTest::addColumn<QString>("line");
			QTest::addColumn<QString>("htmlLine");

			QTest::addRow("Bold") << "**FOO**" << "<b>FOO</b>";
			QTest::addRow("BoldExtraSpace") << "** FOO **" << "<b> FOO </b>";
			QTest::addRow("BoldExtraContentBold") << "**FOO** BAZ **FOO**" << "<b>FOO</b> BAZ <b>FOO</b>";
		}


		void test_replaceControlCharactersWithHtml()
		{
			QFETCH(QString, line);
			QFETCH(QString, htmlLine);

			QCOMPARE(FormattedTextModel::replaceControlCharactersWithHtml(line), htmlLine);
		}


		void test_load_data()
		{
			QTest::addColumn<QString>("textFile");
			QTest::addColumn<int>("rowCount");
			QTest::addColumn<PairList>("modelContent");

			PairList fileContent;
			fileContent << qMakePair(QStringLiteral("This is a header"), FormattedTextModel::HEADER);
			fileContent << qMakePair(QString(), FormattedTextModel::EMPTY);
			fileContent << qMakePair(QStringLiteral("First section"), FormattedTextModel::SECTION);
			fileContent << qMakePair(QString(), FormattedTextModel::EMPTY);
			fileContent << qMakePair(QStringLiteral("Subsection one"), FormattedTextModel::SUBSECTION);
			fileContent << qMakePair(QString(), FormattedTextModel::EMPTY);
			fileContent << qMakePair(QStringLiteral("This multiline text will be concatenated to one text."), FormattedTextModel::REGULARTEXT);
			fileContent << qMakePair(QString(), FormattedTextModel::EMPTY);
			fileContent << qMakePair(QStringLiteral("Subsection <b>two</b>"), FormattedTextModel::SUBSECTION);
			fileContent << qMakePair(QString(), FormattedTextModel::EMPTY);
			fileContent << qMakePair(QStringLiteral("Some list items:"), FormattedTextModel::REGULARTEXT);
			fileContent << qMakePair(QString(), FormattedTextModel::EMPTY);
			fileContent << qMakePair(QStringLiteral("First"), FormattedTextModel::LISTITEM);
			fileContent << qMakePair(QString(), FormattedTextModel::EMPTY);
			fileContent << qMakePair(QStringLiteral("Second"), FormattedTextModel::LISTITEM);
			fileContent << qMakePair(QString(), FormattedTextModel::EMPTY);
			fileContent << qMakePair(QStringLiteral("Third"), FormattedTextModel::LISTITEM);

			PairList fileSeparator;
			fileSeparator << qMakePair(QString(), FormattedTextModel::EMPTY);

			QTest::addRow("formattedText.txt") << ":/qml/formattedText.txt" << 35 << fileContent + fileSeparator + fileContent;
		}


		void test_load()
		{
			QFETCH(QString, textFile);
			QFETCH(int, rowCount);
			QFETCH(PairList, modelContent);

			FormattedTextModel textModel(nullptr);
			QCOMPARE(textModel.loadSeveral({textFile, textFile}), true);
			QCOMPARE(textModel.rowCount(), rowCount);

			for (int i = 0; i < textModel.rowCount(); ++i)
			{
				const auto modelIndex = textModel.index(i);
				QCOMPARE(textModel.data(modelIndex, FormattedTextModel::ContentRole), modelContent.at(i).first);
				QCOMPARE(textModel.data(modelIndex, FormattedTextModel::LineTypeRole), modelContent.at(i).second);
			}
		}


};

QTEST_GUILESS_MAIN(test_FormattedTextModel)
#include "test_FormattedTextModel.moc"
