/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0

SectionPage {
	id: baseItem
	enum ButtonStyle {
		NoButtons,
		AgreeButton,
		DisagreeButton,
		NeutralButton = 4,
		AllButtons = 7 // Combination of all button values
	}

	property alias agreeButton: agreeButton
	property alias agreeText: agreeButton.subText
	property alias disagreeButton: disagreeButton
	property alias disagreeText: disagreeButton.subText
	property alias mainIconSource: image.source
	property alias moreInformationVisible: moreInformation.visible
	property alias neutralButton: neutralButton
	property alias neutralText: neutralButton.subText
	property alias questionSubText: subTextElement.text
	property alias questionText: mainTextElement.text
	property int style: DecisionView.ButtonStyle.AgreeButton | DecisionView.ButtonStyle.DisagreeButton
	property alias tintEnabled: image.tintEnabled

	signal agree
	signal disagree
	signal moreInformationClicked
	signal neutral

	activeFocusOnTab: false

	TintableIcon {
		id: image
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.top
		anchors.verticalCenterOffset: baseItem.height / 4
		height: Style.dimens.status_icon_large
		source: "qrc:///images/info.svg"
		sourceSize.height: Style.dimens.status_icon_large
		tintColor: Style.color.accent
	}
	GText {
		id: mainTextElement
		activeFocusOnTab: true
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: parent.verticalCenter
		horizontalAlignment: Text.AlignHCenter
		textStyle: Style.text.header
		verticalAlignment: Text.AlignVCenter
		visible: mainTextElement.text !== ""
		width: Math.min(parent.width - (2 * Constants.pane_padding), Style.dimens.max_text_width)

		onLinkActivated: baseItem.mainTextLinkActivated()

		FocusFrame {
		}
	}
	GText {
		id: subTextElement
		activeFocusOnTab: true
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: mainTextElement.bottom
		anchors.topMargin: Constants.text_spacing
		horizontalAlignment: Text.AlignHCenter
		textStyle: Style.text.header_secondary
		verticalAlignment: Text.AlignVCenter
		visible: subTextElement.text !== ""
		width: Math.min(parent.width - (2 * Constants.pane_padding), Style.dimens.max_text_width)

		onLinkActivated: baseItem.subTextLinkActivated()

		FocusFrame {
		}
	}
	MoreInformationLink {
		id: moreInformation
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: subTextElement.bottom
		anchors.topMargin: Constants.component_spacing
		visible: false

		onClicked: baseItem.moreInformationClicked()
	}
	RowLayout {
		anchors {
			bottom: parent.bottom
			left: parent.left
			margins: Constants.component_spacing
			right: parent.right
		}
		NavigationButton {
			id: disagreeButton
			Layout.fillWidth: true
			Layout.preferredWidth: baseItem.width / 3
			activeFocusOnTab: true
			buttonType: NavigationButton.Type.Cancel

			//: LABEL DESKTOP
			subText: qsTr("No")
			visible: style & DecisionView.ButtonStyle.DisagreeButton

			onClicked: baseItem.disagree()
		}
		NavigationButton {
			id: neutralButton
			Layout.fillWidth: true
			Layout.preferredWidth: baseItem.width / 3
			activeFocusOnTab: true
			buttonType: NavigationButton.Type.Check

			//: LABEL DESKTOP
			subText: qsTr("Maybe")
			visible: style & DecisionView.ButtonStyle.NeutralButton

			onClicked: baseItem.neutral()
		}
		NavigationButton {
			id: agreeButton
			Layout.fillWidth: true
			Layout.preferredWidth: baseItem.width / 3
			activeFocusOnTab: true
			buttonType: NavigationButton.Type.Check

			//: LABEL DESKTOP
			subText: qsTr("Yes")
			visible: style & DecisionView.ButtonStyle.AgreeButton

			onClicked: baseItem.agree()
		}
	}
}
