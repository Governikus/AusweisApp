/*
 * \copyright Copyright (c) 2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.EnterPasswordView 1.0
import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.TitleBar 1.0

SectionPage {
	id: root

	property bool showPasswordInfo: false
	property bool rootEnabled: true

	signal agree()
	signal disagree()

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP_QML
		text: qsTr("Transport PIN")
		rootEnabled: root.rootEnabled
		showSettings: false
		helpTopic: "setupAssistant"
		onClicked: {
			showPasswordInfo = false
			appWindow.menuBar.updateActions()
		}
	}

	onVisibleChanged: if (visible) showPasswordInfo = false

	DecisionView {
		visible: !showPasswordInfo

		Accessible.name: qsTr("Change Transport PIN setup step")

		mainIconSource: "qrc:///images/reader/default_reader.png"
		tintEnabled: false
		//: INFO DESKTOP_QML Inquiry message if the five-digit Transport PIN should be changed to an ordinary PIN (now).
		questionText: qsTr("Do you want to change your (Transport) PIN now?")
		//: INFO DESKTOP_QML Hint that a six-digit PIN is required to use the online authentication feature of the ID card.
		questionSubText: "%1<br><br><a href=\"#\">%2</a>".arg(qsTr("If you have not already done so you have to change your five-digit Transport PIN to a six-digit PIN before you can use the online-ID function.")).arg(qsTr("More information"))

		onSubTextLinkActivated: {
			showPasswordInfo = true
			appWindow.menuBar.updateActions()
		}

		onAgree: root.agree()
		onDisagree: root.disagree()
	}

	PasswordInfoView {
		id: passwordInfoView

		visible: showPasswordInfo

		rootEnabled: root.rootEnabled

		onClose: {
			showPasswordInfo = false
			appWindow.menuBar.updateActions()
		}
	}
}
