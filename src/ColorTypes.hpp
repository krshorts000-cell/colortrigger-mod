#pragma once
#include <Geode/Geode.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/EditorUI.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/EffectGameObject.hpp>
#include <Geode/binding/ColorAction.hpp>
#include <Geode/binding/GJEffectManager.hpp>
#include <Geode/binding/LevelSettingsObject.hpp>
#include <Geode/binding/GJSpriteColor.hpp>
#include <Geode/binding/UndoObject.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/CCMenuItemToggler.hpp>
#include <Geode/binding/FLAlertLayer.hpp>
#include <cocos2d.h>
#include <optional>
#include <vector>
#include <set>
#include <algorithm>

using namespace geode::prelude;

namespace SystemColors {
    constexpr int BG       = 1000;
    constexpr int LINE     = 1002;
    constexpr int _3DL     = 1003;
    constexpr int OBJ      = 1004;
    constexpr int GROUND1  = 1001;
    constexpr int GROUND2  = 1009;
    constexpr int WHITE    = 1010;

    inline bool isSystem(int id) {
        return id == BG || id == LINE || id == _3DL ||
               id == OBJ || id == GROUND1 || id == GROUND2;
    }
}

constexpr int COLOR_TRIGGER_OBJ_ID = 899;

struct CapturedColor {
    int     channelID   = 0;
    uint8_t r = 255, g = 255, b = 255;
    uint8_t opacity     = 255;
    bool    blending    = false;
    int     copyColorID = 0;
    float   copyHue = 0.f, copySat = 1.f, copyBri = 1.f;

    bool isDefault() const {
        return r == 255 && g == 255 && b == 255 &&
               opacity == 255 && !blending && copyColorID == 0;
    }
};
