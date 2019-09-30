/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0

SectionPage {
	id: baseItem

	property alias questionText: mainTextElement.text
	property alias questionSubText: subTextElement.text
	property string mainIconSource: "qrc:///images/status_info.svg"
	property string agreeText: ""
	property string disagreeText: ""
	property bool addRingAroundIcon: false

	signal agree()
	signal disagree()

	activeFocusOnTab: false

	Image {
		visible: !baseItem.addRingAroundIcon
		height: Style.dimens.status_icon_large
		width: height
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.top
		anchors.verticalCenterOffset: baseItem.height / 4

		source: baseItem.mainIconSource
	}

	StatusIcon {
		visible: baseItem.addRingAroundIcon
		height: Style.dimens.status_icon_large
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.top
		anchors.verticalCenterOffset: baseItem.height / 4

		source: baseItem.mainIconSource
	}

	GText {
		id: mainTextElement

		visible: mainTextElement.text !== ""
		width: Math.min(parent.width - (2 * Constants.pane_padding), Style.dimens.max_text_width)
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: parent.verticalCenter

		Accessible.role: Accessible.Heading
		Accessible.name: text
		activeFocusOnTab: true

		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter

		textStyle: Style.text.header

		FocusFrame {}
	}

	GText {
		id: subTextElement

		visible: subTextElement.text !== ""
		width: Math.min(parent.width - (2 * Constants.pane_padding), Style.dimens.max_text_width)
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: mainTextElement.bottom
		anchors.topMargin: Constants.text_spacing

		Accessible.role: Accessible.Heading
		Accessible.name: text
		activeFocusOnTab: true

		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter

		textStyle: Style.text.normal

		FocusFrame {}
	}

	SetupAssistantButton {
		id: disagreeButton

		width: ApplicationModel.scaleFactor * 120
		anchors.margins: Constants.component_spacing
		anchors.bottom: parent.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.horizontalCenterOffset: -baseItem.width / 5

		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		subText: qsTr("No") + SettingsModel.translationTrigger
		buttonImage: "qrc:///images/cancel.svg"
		onClicked: baseItem.disagree()
	}

	SetupAssistantButton {
		id: agreeButton

		width: ApplicationModel.scaleFactor * 120
		anchors.margins: Constants.component_spacing
		anchors.bottom: parent.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.horizontalCenterOffset: baseItem.width / 5

		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		subText: qsTr("Yes") + SettingsModel.translationTrigger
		buttonImage: "qrc:///images/check.svg"
		onClicked: baseItem.agree()
	}
}
