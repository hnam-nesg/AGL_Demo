import QtQuick
import QtQuick.Effects
import Qt5Compat.GraphicalEffects

Rectangle{
    width: 430; height: 640; color: "#3f6f6f6f"; radius: 10
    BorderColor{
    }
    Rectangle {
        anchors.fill: parent
        scale: 1.01
        radius: 10
        color: "transparent"
        border.width: 3
        layer.enabled: true
        layer.effect: MultiEffect {
            blurEnabled: true
            blur:0.6
        }
        BorderColor{borderWidth: 2}
    }

    Image{
        x: 30; y: 160; width: 370; height: 450
        source: ASSET_BASE + "asset/blue.jpg"
    }
}