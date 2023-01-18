/*
 * \copyright Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
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
	property var textStyle: Style.text.normal_inverse
	property var titleTextStyle: Style.text.title_inverse

	Accessible.description: qsTr("Contact information of the selected provider.")
	Accessible.focusable: true
	Accessible.name: qsTr("Provider contact information")
	Accessible.role: Accessible.Grouping
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
				isOnLightBackground: false
				scope: baseItem
			}
		}
		Repeater {
			id: repeater
			Item {
				readonly property bool showSeparator: index !== 0

				Layout.fillHeight: true
				Layout.fillWidth: true
				Layout.maximumHeight: contactItem.implicitHeight + (showSeparator ? layout.spacing : 0)
				Layout.preferredHeight: contactItem.implicitHeight + (showSeparator ? layout.spacing : 0)

				GSeparator {
					color: baseItem.textStyle.textColor
					visible: showSeparator

					anchors {
						left: parent.left
						right: parent.right
					}
				}
				ProviderContactInfoItem {
					id: contactItem
					//: LABEL DESKTOP
					accessibleText: (!!model.accessibleText ? model.accessibleText : qsTr("Unknown"))
					imageSource: Qt.resolvedUrl(model.iconSource)
					//: LABEL DESKTOP
					itemText: (!!model.text ? model.text : qsTr("Unknown"))
					label: qsTranslate("ProviderModelItem", model.label)
					link: model.link
					textStyle: baseItem.textStyle

					anchors {
						fill: parent
						topMargin: showSeparator ? layout.spacing : 0
					}
				}
			}
		}
		GSpacer {
			Layout.fillHeight: true
		}
	}
}
