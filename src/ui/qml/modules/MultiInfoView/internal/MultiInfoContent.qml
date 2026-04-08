/**
 * Copyright (c) 2022-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

QtObject {
	property list<MultiInfoContentBlock> contentList
	property Component customContent: null
	property string hintBoxesTitle: ""

	//: ALL_PLATFORMS
	property string linkText: qsTr("More information")
	property MultiInfoHintContent multiInfoHintContent: MultiInfoHintContent {
	}
	property string title
}
