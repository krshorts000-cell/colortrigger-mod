#include "ColorTriggerPopup.hpp"
#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>

using namespace geode::prelude;

class $modify(CTG_EditorUI, EditorUI) {
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel)) return false;

        auto* editButtonBar = this->getChildByID("editor-buttons-menu");
        if (!editButtonBar) {
            editButtonBar = this->getChildByType<cocos2d::CCMenu>(0);
        }
        if (editButtonBar) {
            auto* spr = ButtonSprite::create("CT", 24, true, "bigFont.fnt", "GJ_button_01.png", 30.f, 0.6f);
            auto* btn = CCMenuItemSpriteExtra::create(
                spr, this, menu_selector(CTG_EditorUI::onOpenCTG)
            );
            btn->setID("ctg-button"_spr);
            editButtonBar->addChild(btn);
            editButtonBar->updateLayout();
        }
        return true;
    }

    void onOpenCTG(cocos2d::CCObject*) {
        auto* popup = ColorTriggerPopup::create(this);
        if (popup) popup->show();
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
    log::info("Color Trigger Generator loaded.");
}
