/**
 * Copyright (c) 2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts

import Governikus.Animations
import Governikus.Global
import Governikus.Style
import Governikus.Type
import Governikus.View

FlickableSectionPage {
	id: root

	readonly property bool isSelfAuth: ApplicationModel.currentWorkflow === ApplicationModel.Workflow.SELF_AUTHENTICATION
	property bool startedByOnboarding: false

	spacing: Style.dimens.pane_spacing

	GText {
		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter
		//: LABEL ALL_PLATFORMS
		text: qsTr("Authentication was canceled")
		textStyle: Style.text.headline
		visible: text !== ""
	}
	AnimationLoader {
		Layout.alignment: Qt.AlignHCenter
		symbol: Symbol.Type.ERROR
		type: AnimationLoader.STATUS
	}
	GText {
		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter
		//: LABEL ALL_PLATFORMS
		text: qsTr("The authentication is being canceled at the provider (%1)").arg(CertificateDescriptionModel.subjectName)
		textStyle: Style.text.subline
		visible: text !== ""
	}
	GText {
		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter
		//: LABEL ALL_PLATFORMS
		text: qsTr("This may take a few moments.")
		visible: text !== ""
	}
	GProgressBar {
		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		Layout.maximumWidth: Style.dimens.max_text_width
		value: root.isSelfAuth ? 100 : AuthModel.currentState === "StateRedirectBrowser" ? 100 : 50
	}
	GText {
		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter
		text: root.isSelfAuth || AuthModel.currentState === "StateRedirectBrowser" ?
		//: LABEL ALL_PLATFORMS
		qsTr("The authentication was cancelled successfully.") + "<br>" +
		//: LABEL ALL_PLATFORMS
		(AuthModel.statusCode === GlobalStatusCode.Card_Cancellation_By_User ? qsTr("The cancellation was triggered on the card reader.") + "<br>" : "") +
		//: LABEL ALL_PLATFORMS
		qsTr("You may start a new authentication after pressing the button.") : ""
		visible: text !== ""
	}
	GSpacer {
		Layout.fillHeight: true
	}
	GButton {
		Layout.alignment: Qt.AlignHCenter
		enabled: root.isSelfAuth ? true : AuthModel.currentState === "StateRedirectBrowser"
		icon.source: root.isSelfAuth ? "" : "qrc:///images/open_website.svg"
		text: !root.isSelfAuth ?
		//: LABEL ALL_PLATFORMS
		qsTr("Back to provider") : root.startedByOnboarding ?
		//: LABEL ALL_PLATFORM
		qsTr("Back to setup") :
		//: LABEL ALL_PLATFORMS
		qsTr("Back to start page")
		tintIcon: true

		onClicked: root.leaveView()
	}
}
