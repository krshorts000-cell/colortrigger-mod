#pragma once
#include "ColorTypes.hpp"
#include "ColorUtils.hpp"
#include "TriggerFactory.hpp"
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>

class ColorTriggerPopup : public geode::Popup {
public:
    static ColorTriggerPopup* create(EditorUI* editorUI) {
        auto* ret = new ColorTriggerPopup();
        if (ret && ret->init(editorUI)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

protected:
    EditorUI*         m_editorUI    = nullptr;
    LevelEditorLayer* m_editorLayer = nullptr;

    geode::TextInput* m_offsetInput = nullptr;
    geode::TextInput* m_fadeInput   = nullptr;

    CCMenuItemToggler* m_cbBG    = nullptr;
    CCMenuItemToggler* m_cbG1    = nullptr;
    CCMenuItemToggler* m_cbG2    = nullptr;
    CCMenuItemToggler* m_cbLine  = nullptr;
    CCMenuItemToggler* m_cb3DL   = nullptr;
    CCMenuItemToggler* m_cbObj   = nullptr;
    CCMenuItemToggler* m_cbShift = nullptr;

    geode::TextInput* m_chanFrom = nullptr;
    geode::TextInput* m_chanTo   = nullptr;

    bool init(EditorUI* editorUI) {
        if (!Popup::init(420.f, 320.f)) return false;

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

        // Offset
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

        auto* autoBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Auto", 40, true, "bigFont.fnt", "GJ_button_01.png", 24.f, 0.5f),
            this,
            menu_selector(ColorTriggerPopup::onAutoOffset)
        );
        autoBtn->setPosition({midX + 80.f, curY});
        menu->addChild(autoBtn);

        curY -= 32.f;

        // Fade time
        auto* fadeLbl = cocos2d::CCLabelBMFont::create("Fade Time (s):", "bigFont.fnt");
        fadeLbl->setScale(0.4f);
        fadeLbl->setAnchorPoint({0.f, 0.5f});
        fadeLbl->setPosition({20.f, curY});
        m_mainLayer->addChild(fadeLbl);

        m_fadeInput = geode::TextInput::create(80.f, "0.00", "bigFont.fnt");
        m_fadeInput->setPosition({midX + 10.f, curY});
        m_fadeInput->setFilter("0123456789.");
        m_fadeInput->setMaxCharCount(6);
        float defFade = Mod::get()->getSettingValue<double>("default-fade-time");
        m_fadeInput->setString(fmt::format("{:.2f}", defFade));
        m_mainLayer->addChild(m_fadeInput);

        curY -= 36.f;
        addSeparator(curY + 14.f);
        curY -= 6.f;

        // System colors
        auto* sysLbl = cocos2d::CCLabelBMFont::create("System Colors:", "bigFont.fnt");
        sysLbl->setScale(0.42f);
        sysLbl->setAnchorPoint({0.f, 0.5f});
        sysLbl->setPosition({20.f, curY});
        m_mainLayer->addChild(sysLbl);
        curY -= 26.f;

        m_cbBG   = addCheckbox(menu, "BG (1000)",      {50.f,  curY}, true);
        m_cbG1   = addCheckbox(menu, "Ground1 (1001)", {170.f, curY}, true);
        m_cbG2   = addCheckbox(menu, "Ground2 (1009)", {295.f, curY}, true);
        curY -= 24.f;

        m_cbLine = addCheckbox(menu, "Line (1002)",    {50.f,  curY}, true);
        m_cb3DL  = addCheckbox(menu, "3DL (1003)",     {170.f, curY}, true);
        m_cbObj  = addCheckbox(menu, "Obj (1004)",     {295.f, curY}, true);
        curY -= 32.f;

        addSeparator(curY + 14.f);
        curY -= 6.f;

        // Custom range
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

        auto* toLabel = cocos2d::CCLabelBMFont::create("-", "bigFont.fnt");
        toLabel->setScale(0.5f);
        toLabel->setPosition({midX + 20.f, curY});
        m_mainLayer->addChild(toLabel);

        m_chanTo = geode::TextInput::create(50.f, "999", "bigFont.fnt");
        m_chanTo->setPosition({midX + 55.f, curY});
        m_chanTo->setFilter("0123456789");
        m_chanTo->setMaxCharCount(3);
        m_mainLayer->addChild(m_chanTo);

        curY -= 28.f;

        m_cbShift = addCheckbox(menu, "Shift Selected Object Colors", {midX - 80.f, curY}, true);
        curY -= 36.f;

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

    void addSeparator(float y) {
        auto* sep = cocos2d::CCLayerColor::create({0, 0, 0, 80},
            m_mainLayer->getContentSize().width - 30.f, 1.f);
        sep->setPosition({15.f, y});
        m_mainLayer->addChild(sep);
    }

    CCMenuItemToggler* addCheckbox(cocos2d::CCMenu* menu,
                                   const char* text, cocos2d::CCPoint pos,
                                   bool defaultOn) {
        auto* offSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        auto* onSpr  = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");

        auto* cb = CCMenuItemToggler::create(
            offSpr, onSpr, this,
            menu_selector(ColorTriggerPopup::onToggle)
        );
        cb->setScale(0.65f);
        cb->setPosition(pos);
        if (defaultOn) {
            cb->toggle(true);
        }
        menu->addChild(cb);

        auto* lbl = cocos2d::CCLabelBMFont::create(text, "bigFont.fnt");
        lbl->setScale(0.35f);
        lbl->setAnchorPoint({0.f, 0.5f});
        lbl->setPosition({pos.x + 14.f, pos.y});
        m_mainLayer->addChild(lbl);

        return cb;
    }

    void onToggle(cocos2d::CCObject*) {}

    void onAutoOffset(cocos2d::CCObject*) {
        if (!m_editorLayer || !m_editorUI) return;

        int from = getIntField(m_chanFrom, 1);
        int to   = getIntField(m_chanTo,   999);

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
                "No shift needed - all channels in the selection are already free.", "OK")
                ->show();
        }
    }

    void onCancel(cocos2d::CCObject*) {
        this->onClose(nullptr);
    }

    void onCreate(cocos2d::CCObject*) {
        if (!m_editorLayer || !m_editorUI) {
            this->onClose(nullptr);
            return;
        }

        int   offset    = getIntField(m_offsetInput, 0);
        float fade      = getFloatField(m_fadeInput, 0.f);
        int   chanFrom  = getIntField(m_chanFrom, 1);
        int   chanTo    = getIntField(m_chanTo, 999);
        bool  doShift   = m_cbShift->isOn();

        bool wantBG    = m_cbBG->isOn();
        bool wantG1    = m_cbG1->isOn();
        bool wantG2    = m_cbG2->isOn();
        bool wantLine  = m_cbLine->isOn();
        bool want3DL   = m_cb3DL->isOn();
        bool wantObj   = m_cbObj->isOn();

        auto* selected = m_editorUI->getSelectedObjects();
        if (!selected || selected->count() == 0) {
            FLAlertLayer::create("No Selection",
                "Please select at least one object first.", "OK")->show();
            return;
        }

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
        float spawnY = maxY + 15.f;

        auto selUsedAll = ColorUtils::collectUsedChannels(selected);
        std::set<int> customUsed;
        for (int id : selUsedAll) {
            if (!SystemColors::isSystem(id) && id >= chanFrom && id <= chanTo)
                customUsed.insert(id);
        }

        GJEffectManager* em = nullptr;
        if (m_editorLayer->m_levelSettings) {
            em = m_editorLayer->m_levelSettings->m_effectManager;
        }
        if (!em) {
            em = m_editorLayer->m_effectManager;
        }

        std::vector<CapturedColor> toCreate;

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

        for (int id : customUsed) {
            auto cc = ColorUtils::captureChannel(em, id);
            if (!cc.has_value()) continue;
            if (cc->isDefault()) continue;
            int newID = id + (doShift ? offset : 0);
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
                "No active color channels found in the selection.", "OK")->show();
            return;
        }

        if (doShift && offset > 0) {
            shiftObjectColors(selected, offset, chanFrom, chanTo);
        }

        auto created = TriggerFactory::generateColumn(
            m_editorLayer, toCreate, spawnX, spawnY, fade);

        auto* createdArr = cocos2d::CCArray::create();
        for (auto* trig : created) {
            createdArr->addObject(trig);
        }

        TriggerFactory::registerUndo(m_editorLayer, created);

        m_editorUI->deselectAll();
        m_editorUI->selectObjects(createdArr, true);

        this->onClose(nullptr);

        auto msg = fmt::format(
            "<cg>Created {} color trigger(s).</c>\n"
            "Offset: <cy>{}</c> | Fade: <cy>{:.2f}s</c>",
            created.size(), offset, fade);
        FLAlertLayer::create("Done", msg.c_str(), "OK")->show();
    }

    void shiftObjectColors(cocos2d::CCArray* objects, int offset,
                           int chanFrom, int chanTo) {
        if (!objects) return;
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

            if (obj->m_baseColor) {
                obj->m_baseColor->m_colorID = shift(obj->m_baseColor->m_colorID);
            }
            if (obj->m_detailColor) {
                obj->m_detailColor->m_colorID = shift(obj->m_detailColor->m_colorID);
            }

            if (obj->isColorTrigger()) {
                obj->m_targetColor = shift(obj->m_targetColor);
                if (auto* effectObj = typeinfo_cast<EffectGameObject*>(obj)) {
                    effectObj->updateSpecialColor();
                }
            }
        }

        // Use the editor layer's safe batch color update instead of gameplay updateMainColor()
        if (m_editorLayer) {
            m_editorLayer->updateObjectColors(objects);
        }
    }

    int getIntField(geode::TextInput* input, int fallback) {
        if (!input) return fallback;
        auto s = std::string(input->getString());
        try { return std::stoi(s); } catch (...) { return fallback; }
    }

    float getFloatField(geode::TextInput* input, float fallback) {
        if (!input) return fallback;
        auto s = std::string(input->getString());
        try { return std::stof(s); } catch (...) { return fallback; }
    }
};
