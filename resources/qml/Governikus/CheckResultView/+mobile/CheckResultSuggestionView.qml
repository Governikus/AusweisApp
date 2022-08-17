/*
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Global 1.0
import Governikus.ResultView 1.0

ResultView {
	id: root

	property var suggestionData: SuggestionData {}

	resultType: ResultView.Type.IsInfo
	header: suggestionData.header
	title: suggestionData.title
	text: suggestionData.text
	buttonText: suggestionData.continueButtonText
	buttonIcon: suggestionData.continueButtonIcon
	hintText: suggestionData.hintText
	hintButtonText: suggestionData.hintButtonText
	textFormat: suggestionData.textFormat

	onContinueClicked: suggestionData.continueClicked()
	onHintClicked: suggestionData.hintClicked()

	GButton {
		visible: text !== ""

		anchors.horizontalCenter: parent.horizontalCenter

		text: suggestionData.actionButtonText
		icon.source: suggestionData.actionButtonIcon
		tintIcon: true

		onClicked: suggestionData.actionClicked()
	}
}
