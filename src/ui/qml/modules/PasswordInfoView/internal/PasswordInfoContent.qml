/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick

QtObject {
	property url buttonLink: ""
	property string buttonText: ""
	property list<PasswordInfoContentBlock> contentList
	property string hint: ""
	property string hintButtonText: ""
	property string hintTitle: ""

	//: LABEL ALL_PLATFORMS
	property string linkText: qsTr("More information")
	property string title
}
