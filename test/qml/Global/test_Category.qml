/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtTest 1.15

import Governikus.Global 1.0

TestCase {
	id: testCase

	name: "test_Category"
	visible: true
	when: windowShown

	function imageFileExists(pImageFilePath) {
		var image = createTemporaryQmlObject("import QtQuick 2.15; Image {}", testCase)

		if (image === null) {
			return false
		}

		image.source = pImageFilePath
		return (image.status === Image.Loading || image.status === Image.Ready)
	}

	function test_categories_data() {
		return [
			{category: "", color: "#164a8c", string: "Provider"},
			{category: "all", color: "#164a8c", string: "All"},
			{category: "citizen", color: "#851e6b", string: "Citizen services"},
			{category: "insurance", color: "#53428c", string: "Insurances"},
			{category: "finance", color: "#693800", string: "Financials"},
			{category: "other", color: "#00828a", string: "Other services"},
		]
	}

	function test_categories(data) {
		compare(Category.displayString(data.category), data.string)
		compare(Category.displayColor(data.category), data.color)
		verify(imageFileExists(Category.backgroundImageSource(data.category)))

		// An empty category does not need a button
		if (data.category !== "") {
			verify(imageFileExists(Category.buttonImageSource(data.category)))
			verify(imageFileExists(Category.imageSource(data.category)))
		}
	}
}
