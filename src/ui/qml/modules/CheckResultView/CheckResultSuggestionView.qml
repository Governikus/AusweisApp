/**
 * Copyright (c) 2021-2026 Governikus Service GmbH, Germany
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
	hintButtonLink: suggestionData.hintButtonLink
	hintButtonText: suggestionData.hintButtonText
	hintText: suggestionData.hintText
	linkToOpen: suggestionData.linkToOpen
	text: suggestionData.text
	textFormat: suggestionData.textFormat
	title: suggestionData.title

	onContinueClicked: suggestionData.continueClicked()
	onLeaveView: suggestionData.continueClicked()
}
