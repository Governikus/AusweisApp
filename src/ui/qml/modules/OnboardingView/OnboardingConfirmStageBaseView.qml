/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts
import Governikus.Animations
import Governikus.Global
import Governikus.Style

ColumnLayout {
	id: root

	required property ProgressTracker progress
	required property int stage

	signal continueOnboarding

	Keys.onEnterPressed: root.continueOnboarding()
	Keys.onEscapePressed: root.continueOnboarding()
	Keys.onReturnPressed: root.continueOnboarding()

	GText {
		id: titleText

		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter
		text: confirmationData.headerText
		textStyle: Style.text.headline
		wrapMode: Text.WordWrap
	}
	AnimationLoader {
		Layout.alignment: Qt.AlignHCenter
		Layout.topMargin: Style.dimens.pane_spacing
		animated: false
		symbol: confirmationData.animationSymbol
		type: confirmationData.animationType
	}
	GText {
		Layout.topMargin: Style.dimens.pane_spacing
		text: confirmationData.sublineText !== "" ? confirmationData.sublineText :
		//: LABEL ALL_PLATFORMS
		qsTr("Step %1 of %2 was successful").arg(root.progress.currentStage).arg(root.progress.stages)
		textStyle: Style.text.subline
	}
	GText {
		id: descriptionText

		Layout.topMargin: Style.dimens.text_spacing
		text: confirmationData.descriptionText
		textFormat: Text.StyledText
	}
	Hint {
		id: hintItem

		Layout.fillWidth: true
		Layout.topMargin: Style.dimens.pane_spacing
		text: confirmationData.hintText
		//: LABEL ALL_PLATFORMS
		title: qsTr("Hint")
		visible: text !== ""
	}
	GContinueButton {
		Layout.topMargin: Style.dimens.pane_spacing

		onClicked: root.continueOnboarding()
	}
	OnboardingConfirmationViewData {
		id: confirmationData

		stage: root.stage
	}
}
