/*
 * \copyright Copyright (c) 2020-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtTest 1.12

import Governikus.Global 1.0

TestCase {
	id: testCase

	name: "test_Category"
	visible: true
	when: windowShown

	function imageFileExists(pImageFilePath) {
		var image = createTemporaryQmlObject("import QtQuick 2.12; Image {}", testCase)

		if (image === null) {
			return false
		}

		image.source = pImageFilePath
		return (image.status === Image.Loading || image.status === Image.Ready)
	}

	function test_categories_data() {
		return [
			{category: "", color: "#4d7aa4", string: "Provider"},
			{category: "all", color: "#4d7aa4", string: "All"},
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
