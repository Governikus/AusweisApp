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
	property string firstHintButtonLink
	property string firstHintButtonText
	property string firstHintText
	//: ALL_PLATFORMS
	property string firstHintTitle: qsTr("Hint")
	property string header
	property string hintBoxesTitle
	property string linkToOpen
	property url secondHintButtonLink
	property string secondHintButtonText
	property string secondHintText
	property string secondHintTitle: qsTr("Hint")
	property string text
	property int textFormat: Text.AutoText
	property string title

	signal continueClicked
}
