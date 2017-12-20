import QtQuick 2.5

Item {
	id: baseItem
	property color color: "white"
	visible: state !== "hidden"

	Item {
		id: content
		width: parent.width * 0.7
		height: parent.height * 0.7
		anchors.centerIn: parent

		property double itemWidth: width * 0.7
		property double itemHeight: content.height / 12
		property double itemArrowWidth: itemWidth * 0.6
		property double itemArrowMiddleWidth: itemWidth * 0.85
		property double itemArrowWidthDelta: itemArrowWidth / (2 * Math.SQRT2)
		property double itemWidthDelta: itemWidth / (2 * Math.SQRT2)
		property double itemHeightDelta: itemHeight / (2 * Math.SQRT2)

		Rectangle {
			id: r0
			x: (content.width - content.itemWidth) * 0.5
			y: (2+0) / 6 * content.height - content.itemHeight * 0.5
			width: content.itemWidth
			height: content.itemHeight
			antialiasing: true
			transformOrigin: baseItem.state === "cancel" ? Item.Left : Item.Right
			color: baseItem.color
		}

		Rectangle {
			id: r1
			x: (content.width - content.itemWidth) * 0.5
			y: (2+1) / 6 * content.height - content.itemHeight * 0.5
			width: content.itemWidth
			height: content.itemHeight
			antialiasing: true
			transformOrigin: baseItem.state === "cancel" ? Item.Left : Item.Right
			color: baseItem.color
		}

		Rectangle {
			id: r2
			x: (content.width - content.itemWidth) * 0.5
			y: (2+2) / 6 * content.height - content.itemHeight * 0.5
			width: content.itemWidth
			height: content.itemHeight
			antialiasing: true
			transformOrigin: baseItem.state === "cancel" ? Item.Left : Item.Right
			color: baseItem.color
		}
	}

	states: [
		State {
			name: "back"
			PropertyChanges { target: content; rotation: 180 }
			PropertyChanges { target: r0; transformOrigin: Item.Right; rotation: 45; y: (content.height - content.itemHeight) * 0.5 + content.itemHeightDelta; width: content.itemArrowWidth; x: content.itemWidth * 0.5 }
			PropertyChanges { target: r1; opacity: 1; width: content.itemArrowMiddleWidth; x: (content.width - content.itemArrowMiddleWidth - content.itemHeightDelta) * 0.5 }
			PropertyChanges { target: r2; transformOrigin: Item.Right; rotation: -45; y: (content.height - content.itemHeight) * 0.5 - content.itemHeightDelta; width: content.itemArrowWidth; x: content.itemWidth * 0.5 }
		},
		State {
			name: "cancel"
			PropertyChanges { target: content; rotation: 180 }
			PropertyChanges { target: r0; transformOrigin: Item.Left; rotation: 45; x: content.width * 0.5 - content.itemWidthDelta; y: content.height * 0.5 - content.itemWidthDelta }
			PropertyChanges { target: r1; opacity: 0; width: 0; x: (content.width - content.itemArrowMiddleWidth - content.itemHeightDelta) * 0.5 }
			PropertyChanges { target: r2; transformOrigin: Item.Left; rotation: -45; x: content.width * 0.5 - content.itemWidthDelta; y: content.height * 0.5 + content.itemWidthDelta }
		}
	]

	transitions: [
		Transition {
			from: "cancel"
			to: "back"
			SequentialAnimation{
				PropertyAction { target: content; property: "rotation"; value: 0 }
				PropertyAction { targets: [r0, r1, r2]; property: "transformOrigin"; value: Item.Right }
				PropertyAction { targets: [r0, r1, r2]; property: "x"; value: (content.width - content.itemWidth) * 0.5 }
				PropertyAction { target: r0; property: "y"; value: (content.height - content.itemHeight) * 0.5 + content.itemHeightDelta }
				PropertyAction { target: r2; property: "y"; value: (content.height - content.itemHeight) * 0.5 + content.itemHeightDelta }
			}
			RotationAnimation { target: content; direction: RotationAnimation.Clockwise; duration: 300; easing.type: Easing.InOutQuad }
			PropertyAnimation { targets: [r0, r1, r2]; properties: "width, y, x"; duration: 300; easing.type: Easing.InOutQuad }
		},
		Transition {
			from: "back"
			to: "cancel"
			SequentialAnimation{
				PropertyAction { target: content; property: "rotation"; value: 0 }
				PropertyAction { targets: [r0, r1, r2]; property: "transformOrigin"; value: Item.Left }
				PropertyAction { targets: [r0, r2]; property: "width"; value: content.itemWidth }
				PropertyAction { target: r1; property: "width"; value: 0 }
				PropertyAction { targets: [r0, r1, r2]; property: "x"; value: (content.width - content.itemWidth) * 0.5 }
			}
			RotationAnimation { target: content; direction: RotationAnimation.Clockwise; duration: 300; easing.type: Easing.InOutQuad }
			PropertyAnimation { targets: [r0, r1, r2]; properties: "width, y, x"; duration: 300; easing.type: Easing.InOutQuad }
		},
		Transition {
			RotationAnimation { target: content; direction: RotationAnimation.Clockwise; duration: 300; easing.type: Easing.InOutQuad }
			PropertyAnimation { targets: [r0, r1, r2]; properties: "opacity, width, rotation, y, x"; duration: 300; easing.type: Easing.InOutQuad }
		}
	]
}
