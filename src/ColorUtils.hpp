#pragma once
#include "ColorTypes.hpp"

namespace ColorUtils {

inline std::optional<CapturedColor> captureChannel(GJEffectManager* em, int channelID) {
    if (!em) return std::nullopt;

    CapturedColor cc;
    cc.channelID = channelID;

    auto* ca = em->getColorAction(channelID);
    if (ca) {
        cc.r          = static_cast<uint8_t>(ca->m_color.r);
        cc.g          = static_cast<uint8_t>(ca->m_color.g);
        cc.b          = static_cast<uint8_t>(ca->m_color.b);
        float op = ca->m_toOpacity;
        if (op <= 0.f && ca->m_currentOpacity > 0.f) {
            op = ca->m_currentOpacity;
        }
        cc.opacity    = static_cast<uint8_t>(std::clamp(op * 255.f, 0.f, 255.f));
        cc.blending   = ca->m_blending;
        cc.copyColorID = ca->m_copyID;
        cc.copyHue    = ca->m_copyHSV.h;
        cc.copySat    = ca->m_copyHSV.s;
        cc.copyBri    = ca->m_copyHSV.v;
    } else {
        auto col = em->activeColorForIndex(channelID);
        float op = em->activeOpacityForIndex(channelID);
        cc.r       = col.r;
        cc.g       = col.g;
        cc.b       = col.b;
        cc.opacity = static_cast<uint8_t>(std::clamp(op * 255.f, 0.f, 255.f));
    }
    return cc;
}

inline std::set<int> collectUsedChannels(cocos2d::CCArray* objects) {
    std::set<int> used;
    if (!objects) return used;

    for (int i = 0; i < objects->count(); ++i) {
        auto* obj = static_cast<GameObject*>(objects->objectAtIndex(i));
        if (!obj) continue;

        auto addIfValid = [&](int id) {
            if (id > 0 && id != SystemColors::WHITE) {
                used.insert(id);
            }
        };

        if (obj->m_baseColor) {
            addIfValid(obj->m_baseColor->m_colorID);
        }
        if (obj->m_detailColor) {
            addIfValid(obj->m_detailColor->m_colorID);
        }

        if (obj->isColorTrigger()) {
            addIfValid(obj->m_targetColor);
        }
    }
    return used;
}

inline std::set<int> collectAllUsedChannels(LevelEditorLayer* editorLayer) {
    if (!editorLayer) return {};
    return collectUsedChannels(editorLayer->m_objects);
}

inline int findFreeOffset(const std::set<int>& alreadyUsed,
                          const std::set<int>& toShift,
                          int maxCustom = 999) {
    if (toShift.empty()) return 0;
    for (int offset = 1; offset <= 999; ++offset) {
        bool ok = true;
        for (int id : toShift) {
            int shifted = id + offset;
            if (shifted > maxCustom) { ok = false; break; }
            if (alreadyUsed.count(shifted)) { ok = false; break; }
        }
        if (ok) return offset;
    }
    return 0;
}

} // namespace ColorUtils
