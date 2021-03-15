/*
 * \copyright Copyright (c) 2019-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0

SectionPage {
	id: baseItem

	enum ButtonStyle {
		NoButtons = 0,
		AgreeButton = 1,
		DisagreeButton = 2,
		NeutralButton = 4,
		AllButtons = 7 // Combination of all button values
	}

	property alias questionText: mainTextElement.text
	property alias questionSubText: subTextElement.text
	property alias mainIconSource: image.source
	property alias tintEnabled: image.tintEnabled

	property int style: DecisionView.ButtonStyle.AgreeButton | DecisionView.ButtonStyle.DisagreeButton

	property alias agreeText: agreeButton.subText
	property alias agreeButton: agreeButton

	property alias neutralText: neutralButton.subText
	property alias neutralButton: neutralButton

	property alias disagreeText: disagreeButton.subText
	property alias disagreeButton: disagreeButton

	signal agree()
	signal disagree()
	signal neutral()

	signal mainTextLinkActivated()
	signal subTextLinkActivated()

	activeFocusOnTab: false

	TintableIcon {
		id: image

		height: Style.dimens.status_icon_large
		sourceSize.height: Style.dimens.status_icon_large
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.top
		anchors.verticalCenterOffset: baseItem.height / 4

		source: "qrc:///images/info.svg"
		tintColor: Style.color.button_text
	}

	GText {
		id: mainTextElement

		visible: mainTextElement.text !== ""
		width: Math.min(parent.width - (2 * Constants.pane_padding), Style.dimens.max_text_width)
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: parent.verticalCenter

		activeFocusOnTab: true

		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter

		textStyle: Style.text.header_inverse

		onLinkActivated: baseItem.mainTextLinkActivated()

		FocusFrame {}
	}

	GText {
		id: subTextElement

		visible: subTextElement.text !== ""
		width: Math.min(parent.width - (2 * Constants.pane_padding), Style.dimens.max_text_width)
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: mainTextElement.bottom
		anchors.topMargin: Constants.text_spacing

		activeFocusOnTab: true

		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter

		textStyle: Style.text.header_secondary_inverse
		linkColor: Style.text.header_inverse.textColor
		onLinkActivated: baseItem.subTextLinkActivated()

		FocusFrame {}
	}

	RowLayout {

		anchors {
			margins: Constants.component_spacing
			bottom: parent.bottom
			left: parent.left
			right: parent.right
		}

		NavigationButton {
			id: disagreeButton

			visible: style & DecisionView.ButtonStyle.DisagreeButton
			Layout.fillWidth: true
			Layout.preferredWidth: baseItem.width / 3

			activeFocusOnTab: true

			//: LABEL DESKTOP_QML
			subText: qsTr("No")
			buttonType: NavigationButton.Type.Cancel
			onClicked: baseItem.disagree()
		}

		NavigationButton {
			id: neutralButton

			visible: style & DecisionView.ButtonStyle.NeutralButton
			Layout.fillWidth: true
			Layout.preferredWidth: baseItem.width / 3

			activeFocusOnTab: true

			//: LABEL DESKTOP_QML
			subText: qsTr("Maybe")
			buttonType: NavigationButton.Type.Check
			onClicked: baseItem.neutral()
		}

		NavigationButton {
			id: agreeButton

			visible: style & DecisionView.ButtonStyle.AgreeButton
			Layout.fillWidth: true
			Layout.preferredWidth: baseItem.width / 3

			activeFocusOnTab: true

			//: LABEL DESKTOP_QML
			subText: qsTr("Yes")
			buttonType: NavigationButton.Type.Check
			onClicked: baseItem.agree()
		}
	}
}
