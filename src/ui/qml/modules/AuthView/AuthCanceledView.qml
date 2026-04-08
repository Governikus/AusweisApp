/**
 * Copyright (c) 2025-2026 Governikus GmbH & Co. KG, Germany
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

	Heading {
		//: ALL_PLATFORMS
		text: qsTr("Authentication was canceled")
		visible: text !== ""
	}
	AnimationLoader {
		Layout.alignment: Qt.AlignHCenter
		symbol: Symbol.Type.ERROR
		type: AnimationLoader.Type.STATUS
	}
	Subheading {
		Layout.alignment: Style.scanPatternAlignment
		horizontalAlignment: Style.scanPatternAlignment
		//: ALL_PLATFORMS
		text: qsTr("The authentication is being canceled at the provider (%1)").arg(CertificateDescriptionModel.subjectName)
		visible: text !== ""
	}
	GText {
		Layout.alignment: Style.scanPatternAlignment
		horizontalAlignment: Style.scanPatternAlignment
		//: ALL_PLATFORMS
		text: qsTr("This may take a few moments.")
		visible: text !== ""
	}
	GProgressBar {
		//: ALL_PLATFORMS Name of an progress indicator during the cancellation of an authentication read by screen readers
		Accessible.name: qsTr("Cancellation progress")
		Layout.alignment: Qt.AlignHCenter
		Layout.maximumWidth: Style.dimens.max_text_width
		value: root.isSelfAuth ? 100 : AuthModel.currentState === "StateRedirectBrowser" ? 100 : 50
	}
	GText {
		Layout.alignment: Style.scanPatternAlignment
		horizontalAlignment: Style.scanPatternAlignment
		text: root.isSelfAuth || AuthModel.currentState === "StateRedirectBrowser" ?
		//: ALL_PLATFORMS
		qsTr("The authentication was cancelled successfully.") + "<br>" +
		//: ALL_PLATFORMS
		(AuthModel.statusCode === GlobalStatusCode.Card_Cancellation_By_User ? qsTr("The cancellation was triggered on the card reader.") + "<br>" : "") + (Style.is_layout_desktop ?
			//: DESKTOP
			qsTr("You may start a new authentication after clicking the button.") :
			//: MOBILE
			qsTr("You may start a new authentication after tapping the button.")) : ""
		visible: text !== ""
	}
	GButton {
		Layout.alignment: Style.scanPatternAlignment
		enabled: root.isSelfAuth ? true : AuthModel.currentState === "StateRedirectBrowser"
		icon.source: root.isSelfAuth ? "" : "qrc:///images/open_website.svg"
		text: !root.isSelfAuth ?
		//: ALL_PLATFORMS
		qsTr("Back to provider") : root.startedByOnboarding ?
		//: ALL_PLATFORM
		qsTr("Back to setup") :
		//: ALL_PLATFORMS
		qsTr("Back to start page")
		tintIcon: true

		onClicked: root.leaveView()
	}
}
