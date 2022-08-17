/*
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

QtObject {
	signal actionClicked()
	signal continueClicked()
	signal hintClicked()

	property string title
	property string header
	property string text
	property string continueButtonText
	property url continueButtonIcon
	property string actionButtonText
	property url actionButtonIcon
	property string hintText
	property string hintButtonText
	property int textFormat: Text.AutoText
}
