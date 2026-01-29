/**
 * Copyright (c) 2021-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Animations
import Governikus.Global
import Governikus.ProgressView
import Governikus.ResultView
import Governikus.Style
import Governikus.View
import Governikus.TitleBar
import Governikus.Type

FlickableSectionPage {
	id: root

	property alias buttonText: deleteButton.text
	property alias deleteDescriptionText: deleteDescription.text
	property alias popupButtonText: deleteConfirmation.okButtonText
	property alias popupText: deleteConfirmation.text
	property alias popupTitle: deleteConfirmation.title

	signal close
	signal deleteConfirmed

	function pushProgressView(pProperties) {
		push(progressView, pProperties);
	}
	function pushResultView(pProperties) {
		push(resultView, pProperties);
	}

	smartEidUsed: true
	spacing: Style.dimens.pane_spacing

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: root.pop()
	}

	Component {
		id: progressView

		ProgressView {
			progressValue: SmartModel.progress
			smartEidUsed: root.smartEidUsed
			//: MOBILE
			text: qsTr("Please wait a moment.")
			//: MOBILE
			title: qsTr("Smart-eID")
		}
	}
	Component {
		id: resultView

		ResultErrorView {
			property bool success: false

			animationSymbol: success ? Symbol.Type.CHECK : Symbol.Type.ERROR
			animationType: AnimationLoader.Type.STATUS
			mailButtonText: success ? "" :
			//: MOBILE
			qsTr("Send log")
			smartEidUsed: root.smartEidUsed
			//: MOBILE
			title: qsTr("Smart-eID")

			onCancelClicked: root.close()
			onContinueClicked: root.close()
			onMailClicked: logModel.mailLogFile("support@ausweisapp.de", PersonalizationModel.getEmailHeader(), PersonalizationModel.getEmailBody())

			LogModel {
				id: logModel

			}
		}
	}
	TintableIcon {
		Layout.alignment: Qt.AlignHCenter
		source: "qrc:///images/identify.svg"
		sourceSize.height: Style.dimens.header_icon_size
		tintColor: Style.color.image
	}
	GPane {
		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true

		GText {
			id: deleteDescription

		}
		GText {
			//: MOBILE
			text: qsTr("If you want to use that functionality again, you need to set up a new Smart-eID first.")
		}
	}
	GSpacer {
		Layout.fillHeight: true
	}
	GButton {
		id: deleteButton

		Layout.alignment: Qt.AlignHCenter
		buttonColor: Style.color.warning
		icon.source: "qrc:///images/identify.svg"

		//: MOBILE
		text: qsTr("Reset Smart-eID")

		onClicked: deleteConfirmation.open()
	}
	ConfirmationPopup {
		id: deleteConfirmation

		onConfirmed: root.deleteConfirmed()
	}
}
