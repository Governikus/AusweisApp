/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

import Governikus.ResultView

ResultView {
	id: root

	required property SuggestionData suggestionData

	animationSymbol: suggestionData.animationSymbol
	animationType: suggestionData.animationType
	buttonIcon: suggestionData.continueButtonIcon
	buttonText: suggestionData.continueButtonText
	header: suggestionData.header
	hintButtonText: suggestionData.hintButtonText
	hintText: suggestionData.hintText
	hintTitle: suggestionData.hintTitle
	text: suggestionData.text
	textFormat: suggestionData.textFormat
	title: suggestionData.title

	onContinueClicked: suggestionData.continueClicked()
	onHintClicked: suggestionData.hintClicked()
	onLeaveView: suggestionData.continueClicked()
}
