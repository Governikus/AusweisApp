/*
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.NumberModel 1.0


Item {
	id: root

	property alias enableText: enableInfo.text
	property alias enableButtonText: enableButton.text
	property alias titleText: title.text
	property alias subTitleText: subTitle.text
	property alias additionalContent: additionalContentItem.data

	signal enableClicked()

	ColumnLayout {
		anchors.fill: parent

		spacing: 0

		GText {
			id: title

			visible: text !== ""

			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width
			Layout.bottomMargin: Constants.text_spacing
			Layout.alignment: Qt.AlignHCenter

			horizontalAlignment: Text.AlignHCenter
			textStyle: Style.text.header_accent
		}

		GText {
			id: subTitle

			visible: text !== ""

			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width
			Layout.bottomMargin: Constants.component_spacing
			Layout.alignment: Qt.AlignHCenter

			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignTop
			textStyle: Style.text.normal_secondary
		}

		GText {
			id: enableInfo

			visible: text !== ""

			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width
			Layout.bottomMargin: Constants.component_spacing
			Layout.alignment: Qt.AlignHCenter

			verticalAlignment: Text.AlignBottom
			horizontalAlignment: Text.AlignHCenter
			textStyle: Style.text.normal_warning
		}

		Item {
			id: additionalContentItem

			visible: children.length !== 0
			height: childrenRect.height

			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width
			Layout.bottomMargin: Constants.component_spacing
			Layout.alignment: Qt.AlignHCenter
		}

		GSpacer { Layout.fillHeight: true }

		GButton {
			id: enableButton

			visible: text !== ""
			Layout.alignment: Qt.AlignHCenter
			Layout.bottomMargin: Constants.component_spacing

			onClicked: enableClicked()
		}
	}
}
