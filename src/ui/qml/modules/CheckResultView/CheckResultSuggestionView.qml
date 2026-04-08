/**
 * Copyright (c) 2021-2026 Governikus GmbH & Co. KG, Germany
 */

import Governikus.ResultView

ResultView {
	required property SuggestionData suggestionData

	animationSymbol: suggestionData.animationSymbol
	animationType: suggestionData.animationType
	buttonIcon: suggestionData.continueButtonIcon
	buttonText: suggestionData.continueButtonText
	header: suggestionData.header
	hintBoxesTitle: suggestionData.hintBoxesTitle
	hintText: suggestionData.hintText
	linkToOpen: suggestionData.linkToOpen
	text: suggestionData.text
	textFormat: suggestionData.textFormat
	title: suggestionData.title

	onContinueClicked: suggestionData.continueClicked()
	onLeaveView: suggestionData.continueClicked()
}
