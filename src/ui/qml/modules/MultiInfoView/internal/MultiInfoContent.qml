/**
 * Copyright (c) 2022-2026 Governikus GmbH & Co. KG, Germany
 */
import QtQuick

QtObject {
	property list<MultiInfoContentBlock> contentList
	property url firstHintButtonLink: ""
	property string firstHintButtonText: ""
	property string firstHintText: ""
	property string firstHintTitle: ""
	property string hintBoxesTitle: ""

	//: ALL_PLATFORMS
	property string linkText: qsTr("More information")
	property url secondHintButtonLink: ""
	property string secondHintButtonText: ""
	property string secondHintText: ""
	property string secondHintTitle: ""
	property string title
}
