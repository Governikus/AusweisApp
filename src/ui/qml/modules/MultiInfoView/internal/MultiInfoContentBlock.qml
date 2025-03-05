/**
 * Copyright (c) 2022-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Style
import Governikus.Animations

QtObject {
	property int blockAligment: Qt.AlignLeft
	property int blockHeaderAnimation: AnimationLoader.NONE
	property url blockImage
	property string blockTitle
	property var paragraphList
	property var paragraphTextStyle: Style.text.normal
}
