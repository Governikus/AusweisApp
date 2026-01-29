/**
 * Copyright (c) 2022-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Style
import Governikus.Animations

QtObject {
	property int blockAlignment: Qt.AlignLeft
	property int blockHeaderAnimation: AnimationLoader.Type.NONE
	property url blockImage
	property string blockTitle
	property var paragraphList
	property var paragraphTextStyle: Style.text.normal
}
