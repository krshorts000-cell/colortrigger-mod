#include "ColorTriggerPopup.hpp"
#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>

using namespace geode::prelude;

// ════════════════════════════════════════════════════════════════════════════
//  EditorUI hook — adds the button to the top toolbar
// ════════════════════════════════════════════════════════════════════════════
class $modify(CTG_EditorUI, EditorUI) {
    struct Fields {
        CCMenuItemSpriteExtra* ctgButton = nullptr;
    };

    // Called once after the editor's UI is fully built
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel)) return false;

        this->schedule(
            schedule_selector(CTG_EditorUI::postInit_delayed),
            0.f, 0, 0.1f   // fire once after one frame
        );
        return true;
    }

    // We defer adding our button by one frame so all native menus are present
    void postInit_delayed(float) {
        addColorTriggerButton();
    }

    void addColorTriggerButton() {
        // Find the edit-buttons bar (top-left row of icons)
        auto* editButtonBar = this->getChildByID("editor-buttons-menu");
        if (!editButtonBar) {
            // Fallback: search by type/tag
            editButtonBar = this->getChildByType<cocos2d::CCMenu>(0);
        }
        if (!editButtonBar) {
            log::warn("[CTG] Could not find editor-buttons-menu");
            return;
        }

        // Build sprite + label
        auto* sprite = CircleButtonSprite::createWithSpriteFrameName(
            "GJ_colorBtn_001.png",
            1.f,
            CircleBaseColor::Green,
            CircleBaseSize::Small
        );

        // Small "CT" label overlay
        auto* lbl = cocos2d::CCLabelBMFont::create("CT", "bigFont.fnt");
        lbl->setScale(0.35f);
        lbl->setColor({255, 255, 255});
        lbl->setPosition(sprite->getContentSize() / 2);
        sprite->addChild(lbl, 10);

        m_fields->ctgButton = CCMenuItemSpriteExtra::create(
            sprite, this,
            menu_selector(CTG_EditorUI::onOpenCTG)
        );
        m_fields->ctgButton->setID("ctg-button");
        m_fields->ctgButton->setTag(999);

        static_cast<cocos2d::CCMenu*>(editButtonBar)->addChild(
            m_fields->ctgButton);
        static_cast<cocos2d::CCMenu*>(editButtonBar)->updateLayout();
    }

    void onOpenCTG(cocos2d::CCObject*) {
        auto* popup = ColorTriggerPopup::create(this);
        if (popup) popup->show();
    }
};

// ════════════════════════════════════════════════════════════════════════════
//  EditorPauseLayer hook — adds "Create Color Triggers" to the pause menu
// ════════════════════════════════════════════════════════════════════════════
class $modify(CTG_EditorPauseLayer, EditorPauseLayer) {
    bool init(LevelEditorLayer* lel) {
        if (!EditorPauseLayer::init(lel)) return false;

        // Find the main buttons menu in the pause layer
        auto* menu = this->getChildByType<cocos2d::CCMenu>(0);
        if (!menu) return true;

        auto* btn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create(
                "Color Triggers", 180, true,
                "bigFont.fnt", "GJ_button_01.png", 32.f, 0.65f),
            this,
            menu_selector(CTG_EditorPauseLayer::onOpenCTG)
        );

        // Position below the last existing button
        auto children = menu->getChildren();
        float lastY = 0.f;
        if (children && children->count() > 0) {
            auto* last = static_cast<cocos2d::CCNode*>(
                children->objectAtIndex(children->count() - 1));
            lastY = last->getPositionY() - 36.f;
        }
        btn->setPosition({0.f, lastY});
        menu->addChild(btn);

        return true;
    }

    void onOpenCTG(cocos2d::CCObject*) {
        auto* editorLayer = LevelEditorLayer::get();
        if (!editorLayer) return;
        auto* editorUI = editorLayer->m_editorUI;
        if (!editorUI) return;

        // Close pause layer, then open popup
        this->onResume(nullptr);

        // Slight delay so the editor re-activates before we show the modal
        cocos2d::CCDirector::get()->getScheduler()->scheduleBlock(
            [editorUI](float) {
                auto* popup = ColorTriggerPopup::create(editorUI);
                if (popup) popup->show();
            }, this, 0.1f, false, 0.f, 10.f, "ctg_open_popup");
    }
};

// ════════════════════════════════════════════════════════════════════════════
//  Mod entry point
// ════════════════════════════════════════════════════════════════════════════
$on_mod(Loaded) {
    log::info("Color Trigger Generator v{} loaded.",
              Mod::get()->getVersion().toString());
}
