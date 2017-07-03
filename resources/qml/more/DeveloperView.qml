import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0

import "../"
import "../global"


SectionPage {
	id: root
	leftTitleBarAction: TitleBarMenuAction { state: stack.depth > 1 ? "back" : ""; onClicked: pop() }
	headerTitleBarAction: TitleBarAction { text: qsTr("Developer options"); font.bold: true }

	content: Item
	{
		height: pane.height + 2 * Constants.component_spacing
		width: root.width

		Column
		{
			anchors.fill: parent
			anchors.margins: Constants.component_spacing

			Pane {
				id: pane

				GroupBox {
					title: "Change the layout style:"
					height: implicitHeight
					width: implicitWidth

					RowLayout {
						anchors.fill: parent

						RadioButton {
							text: "iOS"
							checked: plugin.platformStyle === text.toLowerCase()
							onCheckedChanged: if (checked) { plugin.applyPlatformStyle(text.toLowerCase()) }
						}
						RadioButton {
							text: "Android"
							checked: plugin.platformStyle === text.toLowerCase()
							onCheckedChanged: if (checked) { plugin.applyPlatformStyle(text.toLowerCase()) }
						}
						RadioButton {
							text: "Tablet,Android"
							checked: plugin.platformStyle === text.toLowerCase()
							onCheckedChanged: if (checked) { plugin.applyPlatformStyle(text.toLowerCase()) }
						}
					}
				}

				GroupBox {
					title: "Developer Mode:"
					height: implicitHeight
					width: implicitWidth

					RowLayout {
						anchors.fill: parent

						RadioButton {
							text: "Enabled"
							checked: settingsModel.developerMode
							onCheckedChanged: if (checked) { settingsModel.developerMode = true }
						}
						RadioButton {
							text: "Disabled"
							checked: !settingsModel.developerMode
							onCheckedChanged: if (checked) { settingsModel.developerMode = false }
						}
					}
				}

				GroupBox {
					title: "Use test uri for selfauthentication:"
					height: implicitHeight
					width: implicitWidth

					RowLayout {
						anchors.fill: parent

						RadioButton {
							text: "Enabled"
							checked: settingsModel.useSelfauthenticationTestUri
							onCheckedChanged: if (checked) { settingsModel.useSelfauthenticationTestUri = true }
						}
						RadioButton {
							text: "Disabled"
							checked: !settingsModel.useSelfauthenticationTestUri
							onCheckedChanged: if (checked) { settingsModel.useSelfauthenticationTestUri = false }
						}
					}
				}
			}
		}
	}
}
