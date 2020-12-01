// swift-tools-version:5.3
import PackageDescription

let package = Package(
        name: "AusweisApp2",
        products: [
            .library(
                    name: "AusweisApp2",
                    targets: ["AusweisApp2"])
        ],
        targets: [
            .binaryTarget(
                    name: "AusweisApp2",
                    path: "AusweisApp2.xcframework"
            )
        ]
)
