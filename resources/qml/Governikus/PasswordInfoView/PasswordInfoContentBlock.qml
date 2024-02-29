/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Animations

QtObject {
	property int blockHeaderAnimation: AnimationLoader.Type.NONE
	property string blockTitle
	property bool headline: false
	property var paragraphList
}
