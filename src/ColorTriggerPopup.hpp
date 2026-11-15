#pragma once
#include "ColorTypes.hpp"
#include "ColorUtils.hpp"
#include "TriggerFactory.hpp"
#include <Geode/ui/Popup.hpp>

/*
 * ColorTriggerPopup
 * -----------------
 * Modal settings window that appears when the user clicks "Create Color Triggers".
 *
 * Layout (approximate):
 *  ┌────────────────────────────────────────────┐
 *  │         Create Color Triggers              │
 *  ├────────────────────────────────────────────┤
 *  │  Color ID Offset  [____0____]  [Auto]      │
 *  │  Fade Time (s)    [__0.00___]              │
 *  │                                            │
 *  │  System Colors                             │
 *  │   [✓] BG (1000)   [✓] Ground1 (1001)      │
 *  │   [✓] Ground2 (1009) [✓] Line (1002)      │
 *  │   [✓] 3DL (1003)  [✓] Obj (1004)          │
 *  │                                            │
 *  │  Custom Channels  [1] – [999]              │
 *  │  [✓] Shift Selected Objects Colors         │
 *  │                                            │
 *  │        [Cancel]      [Create]              │
 *  └────────────────────────────────────────────┘
 */
class ColorTriggerPopup : public geode::Popup<EditorUI*> {
public:
    static ColorTriggerPopup* create(EditorUI* editorUI) {
        auto* ret = new ColorTriggerPopup();
        if (ret && ret->initAnchored(420.f, 320.f, editorUI)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

protected:
    // ── References ──────────────────────────────────────────────────────────
    EditorUI*            m_editorUI      = nullptr;
    LevelEditorLayer*    m_editorLayer   = nullptr;

    // ── UI Nodes ─────────────────────────────────────────────────────────────
    cocos2d::CCLabelBMFont* m_offsetLabel   = nullptr;
    geode::TextInput*        m_offsetInput   = nullptr;
    geode::TextInput*        m_fadeInput     = nullptr;

    // System color checkboxes
    CCMenuItemToggler* m_cbBG      = nullptr;
    CCMenuItemToggler* m_cbG1      = nullptr;
    CCMenuItemToggler* m_cbG2      = nullptr;
    CCMenuItemToggler* m_cbLine    = nullptr;
    CCMenuItemToggler* m_cb3DL     = nullptr;
    CCMenuItemToggler* m_cbObj     = nullptr;
    CCMenuItemToggler* m_cbShift   = nullptr;

    // Custom channel range inputs
    geode::TextInput* m_chanFrom = nullptr;
    geode::TextInput* m_chanTo   = nullptr;

    bool setup(EditorUI* editorUI) override {
        m_editorUI    = editorUI;
        m_editorLayer = LevelEditorLayer::get();

        this->setTitle("Create Color Triggers");

        auto* menu = cocos2d::CCMenu::create();
        menu->setPosition({0, 0});
        m_mainLayer->addChild(menu);

        const float W = m_mainLayer->getContentSize().width;
        const float H = m_mainLayer->getContentSize().height;
        const float midX = W / 2.f;

        float curY = H - 50.f;

        // ── Color ID Offset ─────────────────────────────────────────────────
        auto* offLbl = cocos2d::CCLabelBMFont::create("Color ID Offset:", "bigFont.fnt");
        offLbl->setScale(0.4f);
        offLbl->setAnchorPoint({0.f, 0.5f});
        offLbl->setPosition({20.f, curY});
        m_mainLayer->addChild(offLbl);

        m_offsetInput = geode::TextInput::create(80.f, "0", "bigFont.fnt");
        m_offsetInput->setPosition({midX + 10.f, curY});
        m_offsetInput->setFilter("0123456789");
        m_offsetInput->setMaxCharCount(3);
        m_mainLayer->addChild(m_offsetInput);

        // "Auto" button
        auto* autoBtn = CCMenuItemSpriteExtra::create(
            cocos2d::CCLabelBMFont::create("Auto", "bigFont.fnt"),
            this,
            menu_selector(ColorTriggerPopup::onAutoOffset)
        );
        static_cast<cocos2d::CCLabelBMFont*>(autoBtn->getNormalImage())->setScale(0.4f);
        autoBtn->setPosition({midX + 70.f, curY});
        menu->addChild(autoBtn);

        curY -= 32.f;

        // ── Fade Time ───────────────────────────────────────────────────────
        auto* fadeLbl = cocos2d::CCLabelBMFont::create("Fade Time (s):", "bigFont.fnt");
        fadeLbl->setScale(0.4f);
        fadeLbl->setAnchorPoint({0.f, 0.5f});
        fadeLbl->setPosition({20.f, curY});
        m_mainLayer->addChild(fadeLbl);

        m_fadeInput = geode::TextInput::create(80.f, "0.00", "bigFont.fnt");
        m_fadeInput->setPosition({midX + 10.f, curY});
        m_fadeInput->setFilter("0123456789.");
        m_fadeInput->setMaxCharCount(6);
        // Apply saved default
        float defFade = Mod::get()->getSettingValue<float>("default-fade-time");
        m_fadeInput->setString(fmt::format("{:.2f}", defFade));
        m_mainLayer->addChild(m_fadeInput);

        curY -= 36.f;

        // ── Separator ───────────────────────────────────────────────────────
        addSeparator(curY + 14.f);
        curY -= 6.f;

        auto* sysLbl = cocos2d::CCLabelBMFont::create("System Colors:", "bigFont.fnt");
        sysLbl->setScale(0.42f);
        sysLbl->setAnchorPoint({0.f, 0.5f});
        sysLbl->setPosition({20.f, curY});
        m_mainLayer->addChild(sysLbl);
        curY -= 26.f;

        // Row 1
        m_cbBG   = addCheckbox(menu, "BG (1000)",   {50.f,  curY}, true);
        m_cbG1   = addCheckbox(menu, "Ground1 (1001)", {170.f, curY}, true);
        m_cbG2   = addCheckbox(menu, "Ground2 (1009)", {295.f, curY}, true);
        curY -= 24.f;

        // Row 2
        m_cbLine = addCheckbox(menu, "Line (1002)", {50.f,  curY}, true);
        m_cb3DL  = addCheckbox(menu, "3DL (1003)",  {170.f, curY}, true);
        m_cbObj  = addCheckbox(menu, "Obj (1004)",  {295.f, curY}, true);
        curY -= 32.f;

        // ── Separator ───────────────────────────────────────────────────────
        addSeparator(curY + 14.f);
        curY -= 6.f;

        // ── Custom Channel Range ─────────────────────────────────────────────
        auto* chanLbl = cocos2d::CCLabelBMFont::create("Custom Channels:", "bigFont.fnt");
        chanLbl->setScale(0.42f);
        chanLbl->setAnchorPoint({0.f, 0.5f});
        chanLbl->setPosition({20.f, curY});
        m_mainLayer->addChild(chanLbl);

        m_chanFrom = geode::TextInput::create(50.f, "1", "bigFont.fnt");
        m_chanFrom->setPosition({midX - 15.f, curY});
        m_chanFrom->setFilter("0123456789");
        m_chanFrom->setMaxCharCount(3);
        m_mainLayer->addChild(m_chanFrom);

        auto* toLabel = cocos2d::CCLabelBMFont::create("–", "bigFont.fnt");
        toLabel->setScale(0.5f);
        toLabel->setPosition({midX + 20.f, curY});
        m_mainLayer->addChild(toLabel);

        m_chanTo = geode::TextInput::create(50.f, "999", "bigFont.fnt");
        m_chanTo->setPosition({midX + 55.f, curY});
        m_chanTo->setFilter("0123456789");
        m_chanTo->setMaxCharCount(3);
        m_mainLayer->addChild(m_chanTo);

        curY -= 28.f;

        // ── Shift Colors Checkbox ────────────────────────────────────────────
        m_cbShift = addCheckbox(menu, "Shift Selected Object Colors", {midX, curY}, true);
        curY -= 36.f;

        // ── Buttons ──────────────────────────────────────────────────────────
        addSeparator(curY + 14.f);

        auto* cancelBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Cancel", 80, true, "bigFont.fnt", "GJ_button_01.png", 32.f, 0.8f),
            this,
            menu_selector(ColorTriggerPopup::onCancel)
        );
        cancelBtn->setPosition({midX - 70.f, 24.f});
        menu->addChild(cancelBtn);

        auto* createBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Create", 80, true, "bigFont.fnt", "GJ_button_02.png", 32.f, 0.8f),
            this,
            menu_selector(ColorTriggerPopup::onCreate)
        );
        createBtn->setPosition({midX + 70.f, 24.f});
        menu->addChild(createBtn);

        return true;
    }

    // ── Helper: horizontal separator line ───────────────────────────────────
    void addSeparator(float y) {
        auto* sep = cocos2d::CCLayerColor::create({0, 0, 0, 80},
            m_mainLayer->getContentSize().width - 30.f, 1.f);
        sep->setPosition({15.f, y});
        m_mainLayer->addChild(sep);
    }

    // ── Helper: checkbox with label to the right ─────────────────────────────
    CCMenuItemToggler* addCheckbox(cocos2d::CCMenu* menu,
                                   const char* text, cocos2d::CCPoint pos,
                                   bool defaultOn) {
        auto makeOff = []() -> cocos2d::CCNode* {
            return cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        };
        auto makeOn = []() -> cocos2d::CCNode* {
            return cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
        };

        auto* cb = CCMenuItemToggler::create(
            makeOff(), makeOn(), this,
            menu_selector(ColorTriggerPopup::onToggle)
        );
        cb->setScale(0.65f);
        cb->setPosition(pos);
        if (!defaultOn) cb->toggle(true); // CCMenuItemToggler::toggle(true) = off
        menu->addChild(cb);

        auto* lbl = cocos2d::CCLabelBMFont::create(text, "bigFont.fnt");
        lbl->setScale(0.35f);
        lbl->setAnchorPoint({0.f, 0.5f});
        lbl->setPosition({pos.x + 14.f, pos.y});
        m_mainLayer->addChild(lbl);

        return cb;
    }

    // ── Callback: toggle (no-op; state is read at create time) ───────────────
    void onToggle(cocos2d::CCObject*) {}

    // ── Callback: auto-calculate offset ──────────────────────────────────────
    void onAutoOffset(cocos2d::CCObject*) {
        if (!m_editorLayer) return;

        int from = getIntField(m_chanFrom, 1);
        int to   = getIntField(m_chanTo,   999);

        // Build "selection used" set from the custom range, filtered to [from,to]
        auto* sel = m_editorUI->getSelectedObjects();
        auto selUsed = ColorUtils::collectUsedChannels(sel);
        std::set<int> toShift;
        for (int id : selUsed) {
            if (id >= from && id <= to && !SystemColors::isSystem(id))
                toShift.insert(id);
        }

        auto allUsed = ColorUtils::collectAllUsedChannels(m_editorLayer);
        int offset = ColorUtils::findFreeOffset(allUsed, toShift);

        m_offsetInput->setString(std::to_string(offset));
        if (offset == 0) {
            FLAlertLayer::create("Auto Offset",
                "No shift needed — all channels in the selection are already free.", "OK")
                ->show();
        }
    }

    // ── Callback: Cancel ─────────────────────────────────────────────────────
    void onCancel(cocos2d::CCObject*) { this->onClose(nullptr); }

    // ── Callback: Create ─────────────────────────────────────────────────────
    void onCreate(cocos2d::CCObject*) {
        if (!m_editorLayer || !m_editorUI) {
            this->onClose(nullptr);
            return;
        }

        // ── 1. Read settings from UI ─────────────────────────────────────────
        int   offset    = getIntField(m_offsetInput, 0);
        float fade      = getFloatField(m_fadeInput, 0.f);
        int   chanFrom  = getIntField(m_chanFrom, 1);
        int   chanTo    = getIntField(m_chanTo, 999);
        bool  doShift   = !m_cbShift->isToggled(); // toggled = OFF

        bool wantBG    = !m_cbBG->isToggled();
        bool wantG1    = !m_cbG1->isToggled();
        bool wantG2    = !m_cbG2->isToggled();
        bool wantLine  = !m_cbLine->isToggled();
        bool want3DL   = !m_cb3DL->isToggled();
        bool wantObj   = !m_cbObj->isToggled();

        // ── 2. Collect selected objects ───────────────────────────────────────
        auto* selected = m_editorUI->getSelectedObjects();
        if (!selected || selected->count() == 0) {
            FLAlertLayer::create("No Selection",
                "Please select at least one object first.", "OK")->show();
            return;
        }

        // ── 3. Find spawn X (left edge of selection, 30 units to the left) ────
        float minX = 1e9f, maxY = -1e9f;
        for (int i = 0; i < selected->count(); ++i) {
            auto* obj = static_cast<GameObject*>(selected->objectAtIndex(i));
            if (!obj) continue;
            float lx = obj->getPositionX() - obj->m_objectRadius;
            if (lx < minX) minX = lx;
            float ty = obj->getPositionY() + obj->m_objectRadius;
            if (ty > maxY) maxY = ty;
        }
        float spawnX = minX - 30.f;
        float spawnY = maxY + 15.f; // start just above the selection top

        // ── 4. Collect custom channels used by selection ───────────────────────
        auto selUsedAll = ColorUtils::collectUsedChannels(selected);
        std::set<int> customUsed;
        for (int id : selUsedAll) {
            if (!SystemColors::isSystem(id) && id >= chanFrom && id <= chanTo)
                customUsed.insert(id);
        }

        // ── 5. Gather effect manager & capture colors ─────────────────────────
        GJEffectManager* em = m_editorLayer->m_effectManager;
        std::vector<CapturedColor> toCreate;

        // System colors
        auto addSys = [&](bool want, int chID) {
            if (!want) return;
            auto cc = ColorUtils::captureChannel(em, chID);
            if (cc.has_value()) toCreate.push_back(*cc);
        };
        addSys(wantBG,   SystemColors::BG);
        addSys(wantG1,   SystemColors::GROUND1);
        addSys(wantG2,   SystemColors::GROUND2);
        addSys(wantLine, SystemColors::LINE);
        addSys(want3DL,  SystemColors::_3DL);
        addSys(wantObj,  SystemColors::OBJ);

        // Custom channels (shifted)
        for (int id : customUsed) {
            auto cc = ColorUtils::captureChannel(em, id);
            if (!cc.has_value()) continue;
            if (cc->isDefault()) continue; // skip boring white channels
            int newID = id + offset;
            if (newID > 999) {
                log::warn("Channel {} + offset {} = {} exceeds 999, skipping",
                          id, offset, newID);
                continue;
            }
            cc->channelID = newID;
            toCreate.push_back(*cc);
        }

        if (toCreate.empty()) {
            FLAlertLayer::create("Nothing to Do",
                "No non-default color channels found in the selection.", "OK")->show();
            return;
        }

        // ── 6. Shift color IDs on selected objects (optional) ─────────────────
        if (doShift && offset > 0) {
            shiftObjectColors(selected, offset, chanFrom, chanTo);
        }

        // ── 7. Generate trigger column ────────────────────────────────────────
        auto created = TriggerFactory::generateColumn(
            m_editorLayer, toCreate, spawnX, spawnY, fade);

        // ── 8. Select all created triggers ───────────────────────────────────
        m_editorUI->deselectAll();
        for (auto* trig : created) {
            m_editorUI->selectObject(trig, true);
        }

        // ── 9. Register undo ──────────────────────────────────────────────────
        TriggerFactory::registerUndo(m_editorLayer, created);

        // ── 10. Close popup ───────────────────────────────────────────────────
        this->onClose(nullptr);

        // ── 11. Feedback ──────────────────────────────────────────────────────
        auto msg = fmt::format(
            "<cg>Created {} color trigger(s).</c>\n"
            "Offset: <cy>{}</c> | Fade: <cy>{:.2f}s</c>",
            created.size(), offset, fade);
        FLAlertLayer::create("Done", msg, "OK")->show();
    }

    // ── Shift base/detail/target colorIDs on all selected objects ─────────────
    void shiftObjectColors(cocos2d::CCArray* objects, int offset,
                           int chanFrom, int chanTo) {
        for (int i = 0; i < objects->count(); ++i) {
            auto* obj = static_cast<GameObject*>(objects->objectAtIndex(i));
            if (!obj) continue;

            auto shift = [&](int id) -> int {
                if (id >= chanFrom && id <= chanTo &&
                    !SystemColors::isSystem(id) && id > 0) {
                    return std::min(id + offset, 999);
                }
                return id;
            };

            obj->m_baseColorID   = shift(obj->m_baseColorID);
            obj->m_detailColorID = shift(obj->m_detailColorID);

            // If this is itself a Color Trigger, shift its target too
            if (obj->m_objectID == COLOR_TRIGGER_OBJ_ID) {
                obj->m_targetColorID = shift(obj->m_targetColorID);
            }

            // Refresh the object's display color
            obj->updateMainColor(obj->m_baseColor);
            obj->updateDetailColor(obj->m_detailColor);
        }
    }

    // ── Field helpers ─────────────────────────────────────────────────────────
    int getIntField(geode::TextInput* input, int fallback) {
        if (!input) return fallback;
        auto s = input->getString();
        try { return std::stoi(s); } catch (...) { return fallback; }
    }

    float getFloatField(geode::TextInput* input, float fallback) {
        if (!input) return fallback;
        auto s = input->getString();
        try { return std::stof(s); } catch (...) { return fallback; }
    }
};
