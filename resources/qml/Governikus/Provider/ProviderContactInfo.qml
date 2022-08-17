/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.View 1.0


Item {
	id: baseItem

	property alias contactModel: repeater.model
	property var titleTextStyle: Style.text.title_inverse
	property var textStyle: Style.text.normal_inverse

	Accessible.name: qsTr("Provider contact information")
	Accessible.description: qsTr("Contact information of the selected provider.")
	Accessible.role: Accessible.Grouping
	Accessible.focusable: true

	implicitHeight: layout.implicitHeight
	implicitWidth: layout.implicitWidth

	ColumnLayout {
		id: layout

		anchors.fill: parent

		spacing: Constants.text_spacing

		GText {
			Layout.bottomMargin: Constants.groupbox_spacing

			//: LABEL DESKTOP
			text: qsTr("Contact")
			textStyle: baseItem.titleTextStyle

			FocusFrame {
				scope: baseItem
				isOnLightBackground: false
			}
		}

		Repeater {
			id: repeater

			Item {
				readonly property bool showSeparator: index !== 0

				Layout.fillWidth: true
				Layout.fillHeight: true
				Layout.maximumHeight: contactItem.implicitHeight + (showSeparator ? layout.spacing : 0)
				Layout.preferredHeight: contactItem.implicitHeight + (showSeparator ? layout.spacing : 0)

				GSeparator {
					visible: showSeparator

					anchors {
						left: parent.left
						right: parent.right
					}

					color: baseItem.textStyle.textColor
				}

				ProviderContactInfoItem {
					id: contactItem

					anchors {
						fill: parent
						topMargin: showSeparator ? layout.spacing : 0
					}

					imageSource: Qt.resolvedUrl(model.iconSource)
					//: LABEL DESKTOP
					itemText: (!!model.text ? model.text : qsTr("Unknown"))
					textStyle: baseItem.textStyle
					//: LABEL DESKTOP
					accessibleText: (!!model.accessibleText ? model.accessibleText : qsTr("Unknown"))
					label: qsTranslate("ProviderModelItem", model.label)
					link: model.link
				}
			}
		}

		GSpacer { Layout.fillHeight: true }
	}
}
