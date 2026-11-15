#include "ColorTriggerPopup.hpp"
#include "GridSnapUtils.hpp"
#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/ui/Notification.hpp>

using namespace geode::prelude;

class $modify(CTG_EditorUI, EditorUI) {
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel)) return false;

        auto* editButtonBar = this->getChildByID("editor-buttons-menu");
        if (!editButtonBar) {
            editButtonBar = this->getChildByType<cocos2d::CCMenu>(0);
        }
        if (editButtonBar) {
            // 1. Color Triggers button [CT]
            auto* spr = ButtonSprite::create("CT", 24, true, "bigFont.fnt", "GJ_button_01.png", 30.f, 0.6f);
            auto* btn = CCMenuItemSpriteExtra::create(
                spr, this, menu_selector(CTG_EditorUI::onOpenCTG)
            );
            btn->setID("ctg-button"_spr);
            editButtonBar->addChild(btn);

            // 2. Snap to Grid button [Snap]
            auto* snapSpr = ButtonSprite::create("Snap", 28, true, "bigFont.fnt", "GJ_button_02.png", 30.f, 0.55f);
            auto* snapBtn = CCMenuItemSpriteExtra::create(
                snapSpr, this, menu_selector(CTG_EditorUI::onSnapGrid)
            );
            snapBtn->setID("ctg-snap-button"_spr);
            editButtonBar->addChild(snapBtn);

            editButtonBar->updateLayout();
        }
        return true;
    }

    // Intercept pasting of clipboard & custom objects to fix GD 2.2 bounding-box subpixel offset
    cocos2d::CCArray* pasteObjects(gd::string str, bool withColor, bool noUndo) {
        auto result = EditorUI::pasteObjects(str, withColor, noUndo);
        bool autoSnap = Mod::get()->getSettingValue<bool>("auto-snap-paste");
        if (autoSnap && result && result->count() > 0) {
            GridSnap::snapObjectsToGrid(this, result, false);
        }
        return result;
    }

    void onOpenCTG(cocos2d::CCObject*) {
        auto* popup = ColorTriggerPopup::create(this);
        if (popup) popup->show();
    }

    void onSnapGrid(cocos2d::CCObject*) {
        auto* selected = this->getSelectedObjects();
        if (!selected || selected->count() == 0) {
            Notification::create("Select objects first", NotificationIcon::Warning)->show();
            return;
        }

        bool snapped = GridSnap::snapObjectsToGrid(this, selected, true);
        if (snapped) {
            Notification::create("Snapped to Grid!", NotificationIcon::Success)->show();
        } else {
            Notification::create("Already aligned to grid", NotificationIcon::None)->show();
        }
    }
};

class $modify(CTG_EditorPauseLayer, EditorPauseLayer) {
    bool init(LevelEditorLayer* lel) {
        if (!EditorPauseLayer::init(lel)) return false;

        auto* menu = this->getChildByType<cocos2d::CCMenu>(0);
        if (menu) {
            auto* spr = ButtonSprite::create(
                "Color Triggers", 160, true, "bigFont.fnt", "GJ_button_01.png", 30.f, 0.6f
            );
            auto* btn = CCMenuItemSpriteExtra::create(
                spr, this, menu_selector(CTG_EditorPauseLayer::onOpenCTG)
            );
            btn->setID("ctg-pause-button"_spr);
            btn->setPosition({0.f, -120.f});
            menu->addChild(btn);
        }
        return true;
    }

    void onOpenCTG(cocos2d::CCObject*) {
        auto* editorLayer = m_editorLayer;
        if (!editorLayer) return;
        auto* editorUI = editorLayer->m_editorUI;
        if (!editorUI) return;

        this->onResume(nullptr);

        geode::queueInMainThread([editorUI]() {
            auto* popup = ColorTriggerPopup::create(editorUI);
            if (popup) popup->show();
        });
    }
};

$on_mod(Loaded) {
    log::info("Color Trigger Generator & Grid Snap Fix loaded.");
}
