/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.NumberModel 1.0

Item {
	id: root

	property alias additionalContent: additionalContentItem.data
	property alias enableButtonText: enableButton.text
	property alias enableText: enableInfo.text
	property alias subTitleText: subTitle.text
	property alias titleText: title.text

	signal enableClicked

	clip: true

	GFlickableColumnLayout {
		anchors.fill: parent
		spacing: 0

		GText {
			id: title
			Layout.alignment: Qt.AlignHCenter
			Layout.bottomMargin: Constants.text_spacing
			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width
			horizontalAlignment: Text.AlignHCenter
			textStyle: Style.text.header_accent
			visible: text !== ""
		}
		GText {
			id: subTitle
			Layout.alignment: Qt.AlignHCenter
			Layout.bottomMargin: Constants.component_spacing
			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width
			horizontalAlignment: Text.AlignHCenter
			textStyle: Style.text.normal_secondary
			verticalAlignment: Text.AlignTop
			visible: text !== ""
		}
		GText {
			id: enableInfo
			Layout.alignment: Qt.AlignHCenter
			Layout.bottomMargin: Constants.component_spacing
			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width
			horizontalAlignment: Text.AlignHCenter
			textStyle: Style.text.normal_warning
			verticalAlignment: Text.AlignBottom
			visible: text !== ""
		}
		Item {
			id: additionalContentItem
			Layout.alignment: Qt.AlignHCenter
			Layout.bottomMargin: Constants.component_spacing
			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width
			implicitHeight: childrenRect.height
			visible: children.length !== 0
		}
		GSpacer {
			Layout.fillHeight: true
		}
		GButton {
			id: enableButton
			Layout.alignment: Qt.AlignHCenter
			Layout.bottomMargin: Constants.component_spacing
			visible: text !== ""

			onClicked: enableClicked()
		}
	}
}
