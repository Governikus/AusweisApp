/**
 * Copyright (c) 2021-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Animations

QtObject {
	property int animationSymbol: Symbol.Type.INFO
	property int animationType: AnimationLoader.Type.STATUS
	property url continueButtonIcon
	//: ALL_PLATFORMS
	property string continueButtonText: qsTr("Continue")
	property string header
	property string hintBoxesTitle
	property string hintText
	property string linkToOpen
	property string text
	property int textFormat: Text.AutoText
	property string title

	signal continueClicked
}
