/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.ResultView

ResultView {
	id: root

	required property SuggestionData suggestionData

	buttonIcon: suggestionData.continueButtonIcon
	buttonText: suggestionData.continueButtonText
	header: suggestionData.header
	hintButtonText: suggestionData.hintButtonText
	hintText: suggestionData.hintText
	icon: suggestionData.icon
	text: suggestionData.text
	textFormat: suggestionData.textFormat
	title: suggestionData.title

	onContinueClicked: suggestionData.continueClicked()
	onHintClicked: suggestionData.hintClicked()

	GButton {
		Layout.alignment: Qt.AlignHCenter
		icon.source: suggestionData.actionButtonIcon
		text: suggestionData.actionButtonText
		tintIcon: true
		visible: text !== ""

		onClicked: suggestionData.actionClicked()
	}
}
