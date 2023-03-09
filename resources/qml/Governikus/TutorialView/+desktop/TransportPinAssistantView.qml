/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.TitleBar 1.0
import Governikus.PasswordInfoView 1.0
import Governikus.Type.PasswordType 1.0

SectionPage {
	id: root

	property bool rootEnabled: true
	property bool showPasswordInfo: false

	signal agree
	signal disagree

	titleBarAction: TitleBarAction {
		helpTopic: "setupAssistant"
		rootEnabled: root.rootEnabled
		showSettings: false
		//: LABEL DESKTOP
		text: qsTr("Transport PIN")

		onClicked: {
			showPasswordInfo = false;
			updateTitleBarActions();
		}
	}

	onVisibleChanged: if (visible)
		showPasswordInfo = false

	DecisionView {
		mainIconSource: "qrc:///images/material_lock.svg"
		moreInformationText: infoData.linkText
		moreInformationVisible: true
		//: INFO DESKTOP Hint that a six-digit PIN is required to use the online identification feature of the ID card.
		questionSubText: qsTr("If you have not already done so you have to change your five-digit Transport PIN to a six-digit PIN before you can use the online-ID function.")
		//: INFO DESKTOP Inquiry message if the five-digit Transport PIN should be changed to an ordinary PIN (now).
		questionText: qsTr("Do you want to change your (Transport) PIN now?")
		visible: !showPasswordInfo

		onAgree: root.agree()
		onDisagree: root.disagree()
		onMoreInformationClicked: {
			showPasswordInfo = true;
			updateTitleBarActions();
		}
	}
	PasswordInfoData {
		id: infoData
		contentType: PasswordInfoContent.Type.TRANSPORT_PIN
	}
	PasswordInfoView {
		id: passwordInfoView
		infoContent: infoData
		rootEnabled: root.rootEnabled
		visible: showPasswordInfo

		onClose: {
			showPasswordInfo = false;
			updateTitleBarActions();
		}
	}
}
