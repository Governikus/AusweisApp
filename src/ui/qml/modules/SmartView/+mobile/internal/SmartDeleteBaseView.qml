/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
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
	spacing: Constants.component_spacing

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: pop()
	}

	Component {
		id: progressView

		ProgressView {
			progressValue: SmartModel.progress
			smartEidUsed: root.smartEidUsed
			//: LABEL ANDROID IOS
			subText: qsTr("Please wait a moment.")
			//: LABEL ANDROID IOS
			title: qsTr("Smart-eID")
		}
	}
	Component {
		id: resultView

		ResultErrorView {
			property bool success: false

			icon: success ? "qrc:///images/status_ok_%1.svg".arg(Style.currentTheme.name) : "qrc:///images/status_error_%1.svg".arg(Style.currentTheme.name)
			mailButtonText: success ? "" :
			//: LABEL ANDROID IOS
			qsTr("Send log")
			smartEidUsed: root.smartEidUsed
			//: LABEL ANDROID IOS
			title: qsTr("Smart-eID")

			onCancelClicked: root.close()
			onContinueClicked: root.close()
			onMailClicked: LogModel.mailLog("support@ausweisapp.de", PersonalizationModel.getEmailHeader(), PersonalizationModel.getEmailBody())
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

			width: parent.width
		}
		GText {

			//: LABEL ANDROID IOS
			text: qsTr("If you want to use that functionality again, you need to set up a new Smart-eID first.")
			width: parent.width
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

		//: LABEL ANDROID IOS
		text: qsTr("Reset Smart-eID")

		onClicked: deleteConfirmation.open()
		onFocusChanged: if (focus)
			root.positionViewAtItem(this)
	}
	ConfirmationPopup {
		id: deleteConfirmation

		onConfirmed: root.deleteConfirmed()
	}
}
