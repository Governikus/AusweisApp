/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.ResultView 1.0

ResultView {
	id: root

	property var suggestionData: SuggestionData {
	}

	buttonIcon: suggestionData.continueButtonIcon
	buttonText: suggestionData.continueButtonText
	header: suggestionData.header
	hintButtonText: suggestionData.hintButtonText
	hintText: suggestionData.hintText
	resultType: ResultView.Type.IsInfo
	text: suggestionData.text
	textFormat: suggestionData.textFormat
	title: suggestionData.title

	onContinueClicked: suggestionData.continueClicked()
	onHintClicked: suggestionData.hintClicked()

	GButton {
		anchors.horizontalCenter: parent.horizontalCenter
		icon.source: suggestionData.actionButtonIcon
		text: suggestionData.actionButtonText
		tintIcon: true
		visible: text !== ""

		onClicked: suggestionData.actionClicked()
	}
}
