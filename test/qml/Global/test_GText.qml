/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global
import Governikus.Style

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; GText {}", testCase);
	}
	function test_links() {
		let testObject = createTestObject();
		compare(testObject.link, "");
		ignoreWarning("More than one link found: [href=\"http://www.governikus.de\",href=\"http://www.autentapp.de\"]");
		testObject.text = 'Hello World href="http://www.governikus.de" bla bla bla href="http://www.autentapp.de"';
		compare(testObject.link, "http://www.governikus.de");
		testObject.text = 'no link inside';
		compare(testObject.link, "");
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}
	function test_text() {
		let testObject = createTestObject();
		compare(testObject.text, "", "Initial empty text");
		testObject.text = "test";
		compare(testObject.text, "test", "text: test");
	}
	function test_textStyle() {
		let testObject = createTestObject();
		compare(testObject.textStyle, Style.text.normal, "Initial textStyle: normal");
	}

	name: "test_GText"
	visible: true
	when: windowShown

	GText {
		id: testObject

		text: "initial text"

		onLinkActivated: testObject.text = "link activated"

		TestCase {
			function test_spacePress() {
				testObject.text = "Test <a href=\"#\">Test<\a> Test";
				testObject.forceActiveFocus();
				keyClick(Qt.Key_Space);
				compare(testObject.text, "link activated", "Link activated");
			}

			when: windowShown
		}
	}
	GText {
		id: testObject1

		maximumLineCount: 3
		text: "initial text"

		TestCase {
			function test_effectiveMaxLinesHeight_lineHeight() {
				compare(testObject1.effectiveFirstLineHeight, testObject1.height);
				compare(testObject1.effectiveMaxLinesHeight, testObject1.maximumLineCount * testObject1.height);

				// textStyle should change lineheights
				let height_single = testObject1.effectiveFirstLineHeight;
				let height_multi = testObject1.effectiveMaxLinesHeight;
				testObject1.textStyle = Style.text.title;
				verify(testObject1.effectiveFirstLineHeight !== height_single);
				verify(testObject1.effectiveMaxLinesHeight !== height_multi);

				// lineHeight should change lineheights
				height_single = testObject1.effectiveFirstLineHeight;
				height_multi = testObject1.effectiveMaxLinesHeight;
				testObject1.lineHeight = 2;
				verify(testObject1.effectiveFirstLineHeight !== height_single);
				verify(testObject1.effectiveMaxLinesHeight !== height_multi);

				// textStyle should no longer changelineheights because we
				// replaced the binding of lineHeight to textStyle with 2 before
				height_single = testObject1.effectiveFirstLineHeight;
				height_multi = testObject1.effectiveMaxLinesHeight;
				testObject1.textStyle = Style.text.normal;
				verify(testObject1.effectiveFirstLineHeight === height_single);
				verify(testObject1.effectiveMaxLinesHeight === height_multi);

				// lineHeight should still change lineheights
				height_single = testObject1.effectiveFirstLineHeight;
				height_multi = testObject1.effectiveMaxLinesHeight;
				testObject1.lineHeight = 4;
				verify(testObject1.effectiveFirstLineHeight !== height_single);
				verify(testObject1.effectiveMaxLinesHeight !== height_multi);
			}

			when: windowShown
		}
	}
}
