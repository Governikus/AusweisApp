/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.View
import Governikus.Style
import Governikus.Type

SectionPage {
	id: baseItem

	enum ButtonStyle {
		NoButtons,
		AgreeButton,
		DisagreeButton
	}

	readonly property alias agreeButton: agreeButton
	readonly property alias disagreeButton: disagreeButton
	property alias mainIconSource: image.source
	property alias moreInformationText: moreInformation.text
	property alias moreInformationVisible: moreInformation.visible
	property alias questionSubText: subTextElement.text
	property alias questionText: mainTextElement.text
	property int style: DecisionView.ButtonStyle.AgreeButton | DecisionView.ButtonStyle.DisagreeButton

	signal agree
	signal disagree
	signal moreInformationClicked

	activeFocusOnTab: false

	ColumnLayout {
		anchors.fill: parent
		anchors.margins: Constants.pane_padding
		spacing: Constants.component_spacing

		TintableIcon {
			id: image

			Layout.alignment: Qt.AlignHCenter
			Layout.topMargin: Constants.pane_padding
			source: "qrc:///images/info.svg"
			sourceSize.height: Style.dimens.header_icon_size
			tintColor: Style.color.image
		}
		GText {
			id: mainTextElement

			Layout.alignment: Qt.AlignHCenter
			Layout.maximumWidth: Style.dimens.max_text_width
			activeFocusOnTab: true
			horizontalAlignment: Qt.AlignHCenter
			textStyle: Style.text.headline
			visible: mainTextElement.text !== ""

			onLinkActivated: baseItem.mainTextLinkActivated()

			FocusFrame {
			}
		}
		GText {
			id: subTextElement

			Layout.alignment: Qt.AlignHCenter
			Layout.maximumWidth: Style.dimens.max_text_width
			Layout.topMargin: Constants.text_spacing
			activeFocusOnTab: true
			horizontalAlignment: Qt.AlignHCenter
			textStyle: Style.text.subline
			visible: subTextElement.text !== ""

			onLinkActivated: baseItem.subTextLinkActivated()

			FocusFrame {
			}
		}
		MoreInformationLink {
			id: moreInformation

			Layout.alignment: Qt.AlignHCenter
			Layout.topMargin: Constants.component_spacing
			visible: false

			onClicked: baseItem.moreInformationClicked()
		}
		GSpacer {
			Layout.fillHeight: true
		}
		RowLayout {
			Layout.alignment: Qt.AlignHCenter
			Layout.bottomMargin: Style.dimens.huge_icon_size
			Layout.topMargin: Constants.component_spacing
			spacing: Style.dimens.huge_icon_size

			NavigationButton {
				id: disagreeButton

				activeFocusOnTab: true
				buttonType: NavigationButton.Type.Cancel

				//: LABEL DESKTOP
				text: qsTr("No")
				visible: style & DecisionView.ButtonStyle.DisagreeButton

				onClicked: baseItem.disagree()
			}
			NavigationButton {
				id: agreeButton

				activeFocusOnTab: true
				buttonType: NavigationButton.Type.Check

				//: LABEL DESKTOP
				text: qsTr("Yes")
				visible: style & DecisionView.ButtonStyle.AgreeButton

				onClicked: baseItem.agree()
			}
		}
	}
}
