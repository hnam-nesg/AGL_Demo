import QtQuick

Window {
    id: window
    visibility: Window.FullScreen
    flags: Qt.FramelessWindowHint

    Background{
        Component.onCompleted:{
            homescreen.visible = true
        }
    }

    HomeScreen{id: homescreen; visible: false}

    TopBar{id: topbar}

     BottomBar{
        id: bottombar
        onModeBottom:{
            leftbar.steering_checked = false; leftbar.light_checked = false; leftbar.mirror_checked = false; leftbar.glass_checked = false
        }
    }

    LeftBar{id: leftbar; anchors.top: topbar.bottom
        onModeLeft:{
            bottombar.drive_mode = "white"; bottombar.home = "images/home.png"; bottombar.menu = "images/menu.png"; bottombar.camera = "images/camera.png"; bottombar.fan = "images/fan.png"
        }
    }
}
