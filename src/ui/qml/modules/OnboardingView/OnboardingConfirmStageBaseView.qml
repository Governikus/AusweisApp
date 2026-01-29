/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
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

	spacing: Style.dimens.pane_spacing

	Heading {
		id: titleText

		text: confirmationData.headerText
		wrapMode: Text.WordWrap
	}
	AnimationLoader {
		Layout.alignment: Qt.AlignHCenter
		animated: false
		symbol: confirmationData.animationSymbol
		type: confirmationData.animationType
	}
	Subheading {
		text: confirmationData.sublineText !== "" ? confirmationData.sublineText :
		//: ALL_PLATFORMS
		qsTr("Step %1 of %2 was successful").arg(root.progress.currentStage).arg(root.progress.stages)
	}
	GText {
		id: descriptionText

		text: confirmationData.descriptionText
		textFormat: Text.StyledText
	}
	Hint {
		id: hintItem

		Layout.fillWidth: true
		text: confirmationData.hintText
		//: ALL_PLATFORMS
		title: qsTr("Hint")
		visible: text !== ""
	}
	GContinueButton {
		onClicked: root.continueOnboarding()
	}
	OnboardingConfirmationViewData {
		id: confirmationData

		stage: root.stage
	}
}
