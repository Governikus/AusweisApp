/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15

QtObject {
	property url actionButtonIcon
	property string actionButtonText
	property url continueButtonIcon
	property string continueButtonText
	property string header
	property string hintButtonText
	property string hintText
	property string text
	property int textFormat: Text.AutoText
	property string title

	signal actionClicked
	signal continueClicked
	signal hintClicked
}
