/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.Type
import Governikus.View

Control {
	id: root

	readonly property real eyeWidth: eye.width + eye.Layout.leftMargin + eye.Layout.rightMargin
	property alias number: echoField.text
	property alias passwordLength: echoField.maximumLength

	//: LABEL DESKTOP Screenreader text for the password field
	readonly property string passwordState: qsTr("You entered %1 of %2 digits.").arg(number.length).arg(passwordLength)
	readonly property var text: if (Qt.platform.os === "windows")
		passwordState
	readonly property bool validInput: echoField.acceptableInput

	signal accepted

	function append(number) {
		echoField.insert(echoField.length, number);
	}
	function handleKeyEvent(eventKey, eventModifiers = Qt.NoModifier) {
		if (eventKey >= Qt.Key_0 && eventKey <= Qt.Key_9) {
			root.append(eventKey - Qt.Key_0);
		} else if (eventKey === Qt.Key_Backspace) {
			root.removeLast();
		} else if (eventKey === Qt.Key_Delete) {
			echoField.clear();
		} else if (eventKey === Qt.Key_Paste || (eventKey === Qt.Key_V) && (eventModifiers & Qt.ControlModifier)) {
			echoField.paste();
		} else if ((eventKey === Qt.Key_Enter || eventKey === Qt.Key_Return) && validInput) {
			root.accepted();
			return true;
		} else {
			return false;
		}

		// Otherwise focus is lost if last clicked button gets invisible
		// like 'C' in NumberPad.
		if (visible)
			root.forceActiveFocus();
		return true;
	}
	function removeLast() {
		echoField.remove(echoField.length - 1, echoField.length);
	}

	Layout.maximumWidth: contentItem.Layout.maximumWidth + leftPadding + rightPadding
	Layout.minimumWidth: contentItem.Layout.minimumWidth + leftPadding + rightPadding
	Layout.preferredWidth: implicitWidth

	contentItem: RowLayout {
		id: layout

		spacing: Style.dimens.text_spacing
		z: 2

		GridLayout {
			id: grid

			readonly property int markerWidth: Math.ceil(fontMetrics.averageCharacterWidth * 1.4)

			Accessible.focusable: true
			Accessible.name: (eye.activated ?
				//: LABEL DESKTOP Screenreader text for the password field
				qsTr("The number is visible. Digits entered so far: %1").arg(root.number.split("").join(" ")) :
				//: LABEL DESKTOP Screenreader text for the password field
				qsTr("The number is hidden.")) + (root.text === undefined ? " " + root.passwordState : "")
			Accessible.role: Accessible.StaticText
			Layout.maximumWidth: Layout.preferredWidth
			Layout.minimumWidth: markerWidth
			Layout.preferredWidth: markerWidth + (markerWidth + columnSpacing) * Math.max(5, root.passwordLength - 1)
			activeFocusOnTab: true
			columnSpacing: Style.is_layout_desktop ? Style.dimens.pane_spacing : 5
			columns: Math.max(1, Math.min(1 + (width - markerWidth) / (markerWidth + columnSpacing), root.passwordLength))
			rowSpacing: columnSpacing

			Repeater {
				model: root.passwordLength

				Text {
					id: digit

					required property int index

					Layout.alignment: Qt.AlignHCenter
					Layout.maximumHeight: Layout.preferredHeight
					Layout.maximumWidth: Layout.preferredWidth
					Layout.minimumHeight: Layout.preferredHeight
					Layout.minimumWidth: Layout.preferredWidth
					Layout.preferredHeight: fontMetrics.height + Style.dimens.text_spacing
					Layout.preferredWidth: grid.markerWidth
					color: Style.color.textNormal.basic
					font: fontMetrics.font
					horizontalAlignment: Text.AlignHCenter
					text: eye.activated ? root.number.substr(digit.index, 1) : ""
					verticalAlignment: Text.AlignTop

					Rectangle {
						readonly property int normalHeight: Style.is_layout_desktop ? Style.dimens.border_width * 2 : 1

						color: parent.color
						height: digit.index === root.number.length ? normalHeight * 3 : normalHeight
						width: grid.markerWidth

						anchors {
							bottom: parent.bottom
							horizontalCenter: parent.horizontalCenter
						}
					}
					Rectangle {
						color: parent.color
						height: width
						radius: height / 2
						visible: !eye.activated && root.number.charAt(digit.index) !== ""
						width: fontMetrics.averageCharacterWidth

						anchors {
							centerIn: parent
							verticalCenterOffset: -Style.dimens.text_spacing / 2
						}
					}
				}
			}
		}
		Button {
			id: eye

			property bool activated: false

			background: null
			padding: Style.dimens.text_spacing / 2
			text: (activated ?
				//: LABEL DESKTOP Screenreader text for the eye icon to change the password visibility
				qsTr("Press to hide the number") :
				//: LABEL DESKTOP Screenreader text for the eye icon to change the password visibility
				qsTr("Press to show the number"))

			contentItem: TintableIcon {
				source: eye.activated ? "qrc:///images/eye_visibility_on.svg" : "qrc:///images/eye_visibility_off.svg"
				sourceSize.height: Style.is_layout_desktop ? Style.dimens.icon_size : Style.dimens.small_icon_size
				tintColor: Style.color.textNormal.basic
			}

			onClicked: eye.activated = !eye.activated
			onVisibleChanged: if (visible)
				activated = false

			MouseArea {
				acceptedButtons: Qt.NoButton
				anchors.fill: parent
				cursorShape: Qt.PointingHandCursor
			}
			FocusFrame {
			}
		}
	}

	Keys.onPressed: event => {
		event.accepted = root.handleKeyEvent(event.key, event.modifiers);
	}
	onPasswordLengthChanged: root.number = ""

	FontMetrics {
		id: fontMetrics

		font.pixelSize: Style.is_layout_desktop ? UiPluginModel.scaleFactor * 30 : 24
		font.weight: Font.Bold
	}
	TextInput {
		id: echoField

		maximumLength: 6
		visible: false

		validator: RegularExpressionValidator {
			regularExpression: new RegExp("[0-9]{" + echoField.maximumLength + "}")
		}
	}
	FocusFrame {
		framee: layout
		scope: grid
		z: 1

		MouseArea {
			acceptedButtons: Qt.AllButtons
			anchors.fill: parent
			cursorShape: Qt.PointingHandCursor

			onClicked: mouse => {
				root.forceActiveFocus();
				if (mouse.button === Qt.RightButton || mouse.button === Qt.MiddleButton) {
					echoField.paste();
				}
			}
			onPressAndHold: root.handleKeyEvent(Qt.Key_Paste)
		}
	}
}
