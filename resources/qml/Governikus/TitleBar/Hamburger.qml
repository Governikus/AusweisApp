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
			opacity: 1
			color: baseItem.color
		}

		Rectangle {
			id: r1
			x: (content.width - content.itemWidth) * 0.5
			y: (2+1) / 6 * content.height - content.itemHeight * 0.5
			width: content.itemWidth
			height: content.itemHeight
			antialiasing: true
			opacity: (baseItem.state === "cancel") ? 0 : 1
			color: baseItem.color
		}

		Rectangle {
			id: r2
			x: (content.width - content.itemWidth) * 0.5
			y: (2+2) / 6 * content.height - content.itemHeight * 0.5
			width: content.itemWidth
			height: content.itemHeight
			antialiasing: true
			opacity: 1
			color: baseItem.color
		}
	}

	states: [
		State {
			name: "back"
			PropertyChanges { target: content; rotation: 180 }

			PropertyChanges {
				target: r0
				explicit: true
				transformOrigin: Item.Right
				rotation: 45
				opacity: 1
				x: content.itemWidth * 0.5
				y: (2+1) / 6 * content.height - content.itemHeight * 0.25
				width: content.itemArrowWidth
			}
			PropertyChanges {
				target: r1
				explicit: true
				opacity: 1
				x: (content.width - content.itemArrowMiddleWidth - content.itemHeightDelta) * 0.5
				y: (2+1) / 6 * content.height - content.itemHeight * 0.5
				width: content.itemArrowMiddleWidth
			}
			PropertyChanges {
				target: r2
				explicit: true
				transformOrigin: Item.Right
				opacity: 1
				rotation: -45
				x: content.itemWidth * 0.5
				y: (2+1) / 6 * content.height - content.itemHeight * 0.75
				width: content.itemArrowWidth
			}
		},

		State {
			name: "cancel"
			PropertyChanges { target: content; rotation: 180 }

			PropertyChanges {
				target: r0
				explicit: true
				transformOrigin: Item.Center
				rotation: 45
				opacity: 1
				x: content.width * 0.5 - content.itemWidthDelta
				y: (2+1) / 6 * content.height - content.itemHeight * 0.5
				width: content.itemWidth
			}
			PropertyChanges {
				target: r1
				explicit: true
				transformOrigin: Item.Center
				rotation: 0
				opacity: 0
				width: content.itemWidth
				x: (content.width - content.itemWidth) * 0.5
				y: (2+1) / 6 * content.height - content.itemHeight * 0.5
			}
			PropertyChanges {
				target: r2
				explicit: true
				transformOrigin: Item.Center
				rotation: -45
				opacity: 1
				x: content.width * 0.5 - content.itemWidthDelta
				y: (2+1) / 6 * content.height - content.itemHeight * 0.5
				width: content.itemWidth
			}
		},

		State {
			name: ""
			PropertyChanges { target: content; rotation: 0 }

			PropertyChanges {
				target: r0
				explicit: true
				transformOrigin: Item.Right
				rotation: 0
				opacity: 1
				x: (content.width - content.itemWidth) * 0.5
				y: (2+0) / 6 * content.height - content.itemHeight * 0.5
				width: content.itemWidth
			}
			PropertyChanges {
				target: r1
				explicit: true
				transformOrigin: Item.Right
				rotation: 0
				opacity: 1
				x: (content.width - content.itemWidth) * 0.5
				y: (2+1) / 6 * content.height - content.itemHeight * 0.5
				width: content.itemWidth
			}
			PropertyChanges {
				target: r2
				explicit: true
				transformOrigin: Item.Right
				rotation: 0
				opacity: 1
				x: (content.width - content.itemWidth) * 0.5
				y: (2+2) / 6 * content.height - content.itemHeight * 0.5
				width: content.itemWidth
			}
		}
	]

	transitions: [
		Transition {
			RotationAnimation { target: content; direction: RotationAnimation.Clockwise; duration: 300; easing.type: Easing.InOutQuad }
			PropertyAnimation { targets: [r0, r1, r2]; properties: "opacity, width, rotation, y, x"; duration: 300; easing.type: Easing.InOutQuad }
		}
	]
}
