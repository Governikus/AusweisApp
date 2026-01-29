/**
 * Copyright (c) 2021-2026 Governikus GmbH & Co. KG, Germany
 */

import Governikus.ResultView

ResultView {
	id: root

	required property SuggestionData suggestionData

	animationSymbol: suggestionData.animationSymbol
	animationType: suggestionData.animationType
	buttonIcon: suggestionData.continueButtonIcon
	buttonText: suggestionData.continueButtonText
	firstHintButtonLink: suggestionData.firstHintButtonLink
	firstHintButtonText: suggestionData.firstHintButtonText
	firstHintText: suggestionData.firstHintText
	firstHintTitle: suggestionData.firstHintTitle
	header: suggestionData.header
	hintBoxesTitle: suggestionData.hintBoxesTitle
	linkToOpen: suggestionData.linkToOpen
	secondHintButtonLink: suggestionData.secondHintButtonLink
	secondHintButtonText: suggestionData.secondHintButtonText
	secondHintText: suggestionData.secondHintText
	secondHintTitle: suggestionData.secondHintTitle
	text: suggestionData.text
	textFormat: suggestionData.textFormat
	title: suggestionData.title

	onContinueClicked: suggestionData.continueClicked()
	onLeaveView: suggestionData.continueClicked()
}
