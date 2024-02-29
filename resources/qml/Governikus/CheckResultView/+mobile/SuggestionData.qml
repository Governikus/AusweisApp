/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.ResultView
import Governikus.Style

QtObject {
	property url actionButtonIcon
	property string actionButtonText
	property url continueButtonIcon
	property string continueButtonText
	property string header
	property string hintButtonText
	property string hintText
	property url icon: "qrc:///images/status_info_%1.svg".arg(Style.currentTheme.name)
	property string text
	property int textFormat: Text.AutoText
	property string title

	signal actionClicked
	signal continueClicked
	signal hintClicked
}
