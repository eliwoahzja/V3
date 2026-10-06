#pragma once

#include "ImGui/Call_ImGui.h"
#include "../../ImGui/imgui_settings.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>

extern ImFont* F50;

namespace font {
    extern ImFont* inter_semibold;
}

namespace ui_loading {

// =========================
// LIME GREEN THEME
// =========================
static const ImVec4 limeAccent(0.20f, 1.00f, 0.20f, 1.00f);      // Bright Lime
static const ImVec4 limeDim(0.40f, 0.80f, 0.40f, 0.60f);        // Dimmer Lime
static const ImVec4 limeSoft(0.30f, 1.00f, 0.30f, 0.30f);       // Soft Glow
static const ImVec4 limeText(0.10f, 0.50f, 0.10f, 1.00f);       // Dark Text

struct TechNode {
    ImVec2 pos;
    float angle;
    float speed;
    float pulse;
    int nodeType;
};

struct CircuitLine {
    ImVec2 start;
    ImVec2 end;
    float progress;
    float speed;
    bool active;
};

struct HexGrid {
    ImVec2 center;
    float size;
    float rotation;
    float glowIntensity;
};

static bool showLoadingAnimation = false;
static float loadingAnimationTimer = 0.0f;
static float loadingAnimationDuration = 1.55f;
static std::vector<TechNode> techNodes;
static std::vector<CircuitLine> circuitLines;
static std::vector<HexGrid> hexGrids;
static float animationTime = 0.0f;
static float rotationAngle = 0.0f;
static float pulseWave = 0.0f;
static bool seededRandom = false;

inline float EaseOutCubic(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    const float inv = 1.0f - t;
    return 1.0f - inv * inv * inv;
}

inline float EaseInOutSine(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return -(std::cos(3.14159265f * t) - 1.0f) * 0.5f;
}

inline void SeedRandomOnce() {
    if (!seededRandom) {
        std::srand((unsigned int)std::time(nullptr));
        seededRandom = true;
    }
}

inline void InitParticles(float orbitRadius) {
    SeedRandomOnce();
    techNodes.clear();
    circuitLines.clear();
    hexGrids.clear();

    const int nodeCount = 72;
    for (int i = 0; i < nodeCount; ++i) {
        TechNode node{};
        const float angle = (i / (float)nodeCount) * 2.0f * 3.14159265f;
        const float radialJitter = ((std::rand() % 100) / 100.0f - 0.5f) * orbitRadius * 0.34f;
        const float radius = orbitRadius + radialJitter;
        node.pos = ImVec2(std::cos(angle) * radius, std::sin(angle) * radius);
        node.angle = angle;
        node.speed = 0.55f + ((std::rand() % 100) / 100.0f) * 1.15f;
        node.pulse = (std::rand() % 100) / 100.0f * 6.2831853f;
        node.nodeType = std::rand() % 3;
        techNodes.push_back(node);
    }

    const int lineCount = 56;
    for (int i = 0; i < lineCount; ++i) {
        const int idxA = std::rand() % nodeCount;
        int idxB = std::rand() % nodeCount;
        if (idxA == idxB) {
            idxB = (idxB + 11) % nodeCount;
        }

        CircuitLine line{};
        line.start = techNodes[idxA].pos;
        line.end = techNodes[idxB].pos;
        line.progress = (std::rand() % 100) / 100.0f;
        line.speed = 0.45f + ((std::rand() % 100) / 100.0f) * 1.25f;
        line.active = (std::rand() % 100) < 58;
        circuitLines.push_back(line);
    }

    const int hexCount = 14;
    for (int i = 0; i < hexCount; ++i) {
        HexGrid hex{};
        const float angle = (i / (float)hexCount) * 2.0f * 3.14159265f;
        const float radius = orbitRadius * (0.48f + (i % 4) * 0.18f);
        hex.center = ImVec2(std::cos(angle) * radius, std::sin(angle) * radius);
        hex.size = orbitRadius * (0.10f + (i % 3) * 0.03f);
        hex.rotation = ((std::rand() % 360) / 180.0f) * 3.14159265f;
        hex.glowIntensity = 0.45f + ((std::rand() % 100) / 100.0f) * 0.55f;
        hexGrids.push_back(hex);
    }
}

inline void Start(float durationSeconds = 1.55f) {
    showLoadingAnimation = true;
    loadingAnimationTimer = 0.0f;
    loadingAnimationDuration = std::max(0.4f, durationSeconds);
    animationTime = 0.0f;
    rotationAngle = 0.0f;
    pulseWave = 0.0f;
    InitParticles(132.0f);
}

inline bool IsActive() {
    return showLoadingAnimation;
}

inline float GetProgress() {
    if (loadingAnimationDuration <= 0.0f) {
        return 1.0f;
    }
    return std::clamp(loadingAnimationTimer / loadingAnimationDuration, 0.0f, 1.0f);
}

inline const char* GetStageLabel(float progress) {
    if (progress < 0.34f) {
        return "VALIDATING SESSION";
    }
    if (progress < 0.68f) {
        return "LOADING CONFIGURATION";
    }
    return "OPENING INTERFACE";
}

inline void DrawHexRing(ImDrawList* draw, const ImVec2& center, float size, float rotation, ImU32 color, float thickness) {
    for (int i = 0; i < 6; ++i) {
        const float a1 = rotation + (i / 6.0f) * 2.0f * 3.14159265f;
        const float a2 = rotation + ((i + 1) / 6.0f) * 2.0f * 3.14159265f;
        const ImVec2 p1(center.x + std::cos(a1) * size, center.y + std::sin(a1) * size);
        const ImVec2 p2(center.x + std::cos(a2) * size, center.y + std::sin(a2) * size);
        draw->AddLine(p1, p2, color, thickness);
    }
}

inline void DrawAnimation(ImDrawList* draw, const ImVec2& center, float radius, float progress) {
    const float dt = ImGui::GetIO().DeltaTime;
    animationTime += dt;
    rotationAngle += dt * 0.85f;
    pulseWave = std::sin(animationTime * 3.25f) * 0.5f + 0.5f;

    const ImU32 accent = ImGui::GetColorU32(limeAccent);
    const ImU32 accentSoft = ImGui::GetColorU32(ImVec4(limeAccent.x, limeAccent.y, limeAccent.z, 0.58f));
    const ImU32 accentGlow = ImGui::GetColorU32(ImVec4(limeAccent.x * 0.92f, limeAccent.y * 0.92f, limeAccent.z * 0.92f, 0.22f));
    const ImU32 accentDark = ImGui::GetColorU32(ImVec4(limeAccent.x * 0.26f, limeAccent.y * 0.26f, limeAccent.z * 0.26f, 0.82f));
    const ImU32 accentBright = ImGui::GetColorU32(ImVec4(0.78f, 1.00f, 0.84f, 0.92f));
    const ImU32 panelShade = IM_COL32(0, 0, 0, 214);

    for (int layer = 5; layer >= 0; --layer) {
        const float expand = radius * (0.92f + layer * 0.08f);
        const float alpha = 0.05f - layer * 0.0065f;
        draw->AddCircleFilled(center, expand, ImGui::GetColorU32(ImVec4(limeAccent.x * 0.34f, limeAccent.y * 0.34f, limeAccent.z * 0.34f, ImMax(0.01f, alpha))), 64);
    }
    draw->AddCircleFilled(center, radius * 0.74f, panelShade, 48);

    for (auto& hex : hexGrids) {
        hex.rotation += dt * (0.18f + hex.glowIntensity * 0.12f);
        const float pulse = 0.72f + std::sin(animationTime * 1.9f + hex.glowIntensity * 4.5f) * 0.28f;
        const ImVec2 hexCenter(center.x + hex.center.x * 0.9f, center.y + hex.center.y * 0.9f);
        DrawHexRing(draw, hexCenter, hex.size + 6.0f, hex.rotation, ImGui::GetColorU32(ImVec4(limeAccent.x * 0.42f, limeAccent.y * 0.42f, limeAccent.z * 0.42f, 0.12f * pulse)), 1.0f);
        DrawHexRing(draw, hexCenter, hex.size, hex.rotation, ImGui::GetColorU32(ImVec4(limeAccent.x, limeAccent.y, limeAccent.z, 0.36f * pulse)), 1.5f);
    }

    for (auto& line : circuitLines) {
        if (line.active) {
            line.progress += dt * line.speed * 0.42f;
            if (line.progress >= 1.0f) {
                line.progress = 0.0f;
                line.active = (std::rand() % 100) < 70;
            }
        } else if ((std::rand() % 1000) < 10) {
            line.active = true;
            line.progress = 0.0f;
        }

        const ImVec2 lineStart(center.x + line.start.x, center.y + line.start.y);
        const ImVec2 lineEnd(center.x + line.end.x, center.y + line.end.y);
        draw->AddLine(lineStart, lineEnd, ImGui::GetColorU32(ImVec4(limeAccent.x * 0.22f, limeAccent.y * 0.22f, limeAccent.z * 0.22f, 0.12f)), 1.0f);

        if (line.active && line.progress > 0.0f) {
            const ImVec2 current(
                lineStart.x + (lineEnd.x - lineStart.x) * line.progress,
                lineStart.y + (lineEnd.y - lineStart.y) * line.progress
            );
            draw->AddLine(lineStart, current, accentGlow, 1.65f);
            draw->AddCircleFilled(current, 2.2f, accentBright, 12);
        }
    }

    for (auto& node : techNodes) {
        node.angle += dt * node.speed * 0.18f;
        node.pulse += dt * (1.4f + node.speed * 0.4f);

        const float nodeRadius = radius * 0.86f + std::sin(node.pulse) * radius * 0.08f;
        node.pos.x = std::cos(node.angle) * nodeRadius;
        node.pos.y = std::sin(node.angle) * nodeRadius;

        const ImVec2 nodePos(center.x + node.pos.x, center.y + node.pos.y);
        const float pulse = 0.75f + std::sin(node.pulse) * 0.25f;

        switch (node.nodeType) {
        case 0:
            draw->AddCircleFilled(nodePos, 2.8f * pulse, accent, 10);
            draw->AddCircle(nodePos, 5.0f * pulse, accentSoft, 16, 1.15f);
            break;
        case 1:
            draw->AddRectFilled(
                ImVec2(nodePos.x - 2.4f * pulse, nodePos.y - 2.4f * pulse),
                ImVec2(nodePos.x + 2.4f * pulse, nodePos.y + 2.4f * pulse),
                IM_COL32(224, 213, 255, 190) // White
            );
            break;
        default:
            draw->AddTriangleFilled(
                ImVec2(nodePos.x, nodePos.y - 3.4f * pulse),
                ImVec2(nodePos.x - 3.0f * pulse, nodePos.y + 2.2f * pulse),
                ImVec2(nodePos.x + 3.0f * pulse, nodePos.y + 2.2f * pulse),
                accentDark
            );
            break;
        }
    }

    const int segments = 76;
    const float mainArcLength = 0.72f;
    const float startAngle = rotationAngle;
    const float endAngle = startAngle + (2.0f * 3.14159265f * mainArcLength);

    for (int ring = 0; ring < 3; ++ring) {
        const float ringRadius = radius - ring * 16.0f;
        const float thickness = 3.1f - ring * 0.55f;
        const float alpha = 230.0f - ring * 60.0f;

        for (int i = 0; i < segments; ++i) {
            const float t0 = i / (float)segments;
            const float t1 = (i + 1) / (float)segments;
            const float a0 = startAngle + (endAngle - startAngle) * t0;
            const float a1 = startAngle + (endAngle - startAngle) * t1;
            const ImVec2 p0(center.x + std::cos(a0) * ringRadius, center.y + std::sin(a0) * ringRadius);
            const ImVec2 p1(center.x + std::cos(a1) * ringRadius, center.y + std::sin(a1) * ringRadius);
            const float fade = 0.18f + t0 * 0.82f;
            draw->AddLine(p0, p1, ImGui::GetColorU32(ImVec4(limeAccent.x, limeAccent.y, limeAccent.z, (alpha / 255.0f) * fade)), thickness);
        }
    }

    const float progressRadius = radius + 24.0f;
    const float progressSweep = 2.0f * 3.14159265f * (0.1f + 0.9f * EaseInOutSine(progress));
    const float progressStart = -1.5707963f;
    for (int i = 0; i < 54; ++i) {
        const float t0 = i / 54.0f;
        const float t1 = (i + 1) / 54.0f;
        const float a0 = progressStart + progressSweep * t0;
        const float a1 = progressStart + progressSweep * t1;
        const ImVec2 p0(center.x + std::cos(a0) * progressRadius, center.y + std::sin(a0) * progressRadius);
        const ImVec2 p1(center.x + std::cos(a1) * progressRadius, center.y + std::sin(a1) * progressRadius);
        draw->AddLine(p0, p1, ImGui::GetColorU32(ImVec4(0.78f, 1.00f, 0.84f, 0.40f + 0.24f * t0)), 2.2f);
    }

    for (int i = 0; i < 10; ++i) {
        const float angle = rotationAngle * 0.76f + (i / 10.0f) * 2.0f * 3.14159265f;
        const float inner = radius * 0.48f;
        const float outer = radius * (0.65f + std::sin(animationTime * 1.6f + i) * 0.04f);
        const ImVec2 p0(center.x + std::cos(angle) * inner, center.y + std::sin(angle) * inner);
        const ImVec2 p1(center.x + std::cos(angle) * outer, center.y + std::sin(angle) * outer);
        draw->AddLine(p0, p1, ImGui::GetColorU32(ImVec4(limeAccent.x * 0.62f, limeAccent.y * 0.62f, limeAccent.z * 0.62f, 0.42f)), 1.35f);
    }

    const float corePulse = 0.76f + pulseWave * 0.24f;
    draw->AddCircleFilled(center, 14.0f * corePulse, IM_COL32(0, 0, 0, 236), 20);
    draw->AddCircle(center, 18.0f * corePulse, accentSoft, 24, 2.0f);
    draw->AddCircleFilled(center, 6.5f * corePulse, accentBright, 20);
}

inline bool RenderWindow(ImTextureID backgroundTexture = nullptr) {
    if (!showLoadingAnimation) {
        return false;
    }
    IM_UNUSED(backgroundTexture);

    ImGuiIO& io = ImGui::GetIO();
    const ImVec2 viewportCenter = ImGui::GetMainViewport()->GetCenter();
    const ImVec2 panelSize(
        std::min(720.0f, std::max(520.0f, io.DisplaySize.x - 150.0f)),
        std::min(540.0f, std::max(420.0f, io.DisplaySize.y - 70.0f))
    );

    ImGui::SetNextWindowPos(viewportCenter, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(panelSize, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.0f);

    bool finished = false;

    if (ImGui::Begin("##astral_loading_transition", nullptr,
                     ImGuiWindowFlags_NoBackground |
                     ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoMove))
    {
        const ImVec2 pos = ImGui::GetWindowPos();
        ImDrawList* draw = ImGui::GetWindowDrawList();
        const float progress = GetProgress();
        const ImVec2 panelMax(pos.x + panelSize.x, pos.y + panelSize.y);
        const float uiScale = ImClamp(std::min(panelSize.x / 1030.0f, panelSize.y / 680.0f), 0.58f, 1.0f);
        auto S = [&](float v) { return v * uiScale; };
        const ImVec2 innerMin(pos.x + S(22.0f), pos.y + S(20.0f));
        const ImVec2 innerMax(panelMax.x - S(22.0f), panelMax.y - S(22.0f));
        const float innerW = innerMax.x - innerMin.x;
        const float innerH = innerMax.y - innerMin.y;

        draw->AddRectFilled(pos, panelMax, IM_COL32(0, 0, 0, 255), S(18.0f));
        draw->AddRectFilled(pos, panelMax, IM_COL32(3, 3, 7, 255), S(18.0f));
        draw->AddRectFilledMultiColor(
            pos,
            panelMax,
            IM_COL32(0, 0, 0, 0),
            ImGui::GetColorU32(ImVec4(limeAccent.x * 0.48f, limeAccent.y * 0.48f, limeAccent.z * 0.48f, 0.035f)),
            IM_COL32(0, 0, 0, 255),
            ImGui::GetColorU32(ImVec4(limeAccent.x * 0.58f, limeAccent.y * 0.58f, limeAccent.z * 0.58f, 0.045f))
        );
        draw->AddRect(pos, panelMax, ImGui::GetColorU32(ImVec4(limeAccent.x, limeAccent.y, limeAccent.z, 0.40f)), S(18.0f), 0, S(1.6f));
        draw->AddRect(ImVec2(pos.x + S(14.0f), pos.y + S(14.0f)), ImVec2(panelMax.x - S(14.0f), panelMax.y - S(14.0f)), IM_COL32(255, 255, 255, 28), S(10.0f), 0, S(1.0f));

        // =========================
        // ADDED: SCANNING LINES
        // =========================
        for (int i = 0; i < 8; ++i) {
            float y = pos.y + fmod(animationTime * (40.0f + i * 5.0f), panelSize.y);
            float alpha = 0.10f + 0.05f * sin(animationTime * 6.0f + i * 1.5f);
            draw->AddLine(ImVec2(pos.x, y), ImVec2(panelMax.x, y), ImGui::GetColorU32(ImVec4(limeAccent.x, limeAccent.y, limeAccent.z, alpha)), 1.5f);
        }

        for (float y = innerMin.y + S(110.0f); y < innerMax.y - S(150.0f); y += S(14.0f)) {
            for (float x = innerMin.x + innerW * 0.57f; x < innerMax.x - S(58.0f); x += S(14.0f)) {
                draw->AddCircleFilled(ImVec2(x, y), S(1.0f), IM_COL32(98, 86, 128, 46), 6);
            }
        }

        ImFont* titleFont = F50 ? F50 : (font::inter_semibold ? font::inter_semibold : ImGui::GetFont());
        ImFont* labelFont = font::inter_semibold ? font::inter_semibold : ImGui::GetFont();
        const float labelSize = labelFont->FontSize * 0.94f * uiScale;
        const float bodySize = labelFont->FontSize * 1.08f * uiScale;
        animationTime += io.DeltaTime;

        const float leftX = innerMin.x + S(42.0f);
        const float topY = innerMin.y + S(42.0f);
        draw->AddText(labelFont, labelSize * 1.25f, ImVec2(leftX, topY), IM_COL32(170, 165, 196, 245), "AUTHORIZING");
        draw->AddText(titleFont, F50 ? S(48.0f) : titleFont->FontSize * 2.25f * uiScale, ImVec2(leftX, topY + S(44.0f)), IM_COL32(255, 255, 255, 255), "SESSION");
        draw->AddText(titleFont, F50 ? S(48.0f) : titleFont->FontSize * 2.25f * uiScale, ImVec2(leftX, topY + S(98.0f)), IM_COL32(255, 255, 255, 245), "DARMODZ V5");

        const ImVec2 orb(innerMin.x + innerW * 0.27f, innerMin.y + innerH * 0.57f);
        const float orbR = std::min(innerW, innerH) * 0.175f;
        const float pulseCore = 0.5f + 0.5f * std::sin(animationTime * 3.4f);
        for (int glow = 7; glow >= 0; --glow) {
            const float glowPulse = 1.0f + pulseCore * 0.030f;
            draw->AddCircleFilled(orb, orbR * (0.74f + glow * 0.09f) * glowPulse, ImGui::GetColorU32(ImVec4(limeAccent.x * 0.24f, limeAccent.y * 0.24f, limeAccent.z * 0.24f, 0.038f - glow * 0.0028f)), 96);
        }
        for (int i = 0; i < 13; ++i) {
            const float r = orbR * (0.46f + i * 0.055f);
            draw->AddCircle(orb, r, ImGui::GetColorU32(ImVec4(limeAccent.x, limeAccent.y, limeAccent.z, 0.070f + (i % 3) * 0.025f)), 128, S(1.0f));
        }
        auto drawArc = [&](float radius, float start, float sweep, ImU32 color, float thickness, int steps) {
            ImVec2 prev(orb.x + std::cos(start) * radius, orb.y + std::sin(start) * radius);
            for (int s = 1; s <= steps; ++s) {
                const float a = start + sweep * (s / (float)steps);
                ImVec2 cur(orb.x + std::cos(a) * radius, orb.y + std::sin(a) * radius);
                draw->AddLine(prev, cur, color, thickness);
                prev = cur;
            }
        };
        auto drawScanBeam = [&](float radius, float angle, float length, ImU32 color) {
            const float half = length * 0.5f;
            ImVec2 pts[3] = {
                orb,
                ImVec2(orb.x + std::cos(angle - half) * radius, orb.y + std::sin(angle - half) * radius),
                ImVec2(orb.x + std::cos(angle + half) * radius, orb.y + std::sin(angle + half) * radius)
            };
            draw->AddConvexPolyFilled(pts, 3, color);
        };
        drawScanBeam(orbR * 0.78f, -1.25f + animationTime * 0.62f, 0.28f, ImGui::GetColorU32(ImVec4(limeAccent.x, limeAccent.y, limeAccent.z, 0.055f)));
        drawArc(orbR + S(30.0f), -1.55f + animationTime * 0.35f, 1.05f, IM_COL32(238, 206, 255, 235), S(7.0f), 28);
        drawArc(orbR + S(30.0f), 2.70f + animationTime * 0.35f, 0.82f, ImGui::GetColorU32(ImVec4(limeAccent.x, limeAccent.y, limeAccent.z, 0.78f)), S(5.0f), 24);
        drawArc(orbR + S(16.0f), 0.40f - animationTime * 0.80f, 0.78f, ImGui::GetColorU32(ImVec4(limeAccent.x, limeAccent.y, limeAccent.z, 0.45f)), S(2.0f), 20);
        drawArc(orbR - S(10.0f), 2.10f + animationTime * 1.10f, 1.20f, ImGui::GetColorU32(ImVec4(0.35f, 1.00f, 0.45f, 0.32f)), S(1.6f), 24);
        for (int i = 0; i < 64; ++i) {
            if ((i % 3) == 1) {
                continue;
            }
            const float a0 = -animationTime * 0.45f + i * 0.0981748f;
            const float a1 = a0 + 0.038f;
            const ImVec2 p0(orb.x + std::cos(a0) * (orbR + S(48.0f)), orb.y + std::sin(a0) * (orbR + S(48.0f)));
            const ImVec2 p1(orb.x + std::cos(a1) * (orbR + S(48.0f)), orb.y + std::sin(a1) * (orbR + S(48.0f)));
            draw->AddLine(p0, p1, ImGui::GetColorU32(ImVec4(limeAccent.x, limeAccent.y, limeAccent.z, 0.52f)), S(3.0f));
        }
        for (int i = 0; i < 48; ++i) {
            const float ang = animationTime * 0.25f + i * 0.1308997f;
            const float r0 = orbR + S((i % 4 == 0) ? 34.0f : 38.0f);
            const float r1 = r0 + S((i % 4 == 0) ? 14.0f : 7.0f);
            const ImVec2 p0(orb.x + std::cos(ang) * r0, orb.y + std::sin(ang) * r0);
            const ImVec2 p1(orb.x + std::cos(ang) * r1, orb.y + std::sin(ang) * r1);
            draw->AddLine(p0, p1, ImGui::GetColorU32(ImVec4(0.35f, 1.00f, 0.45f, (i % 4 == 0) ? 0.46f : 0.20f)), S((i % 4 == 0) ? 1.7f : 1.0f));
        }
        draw->AddCircleFilled(orb, orbR * 0.78f, IM_COL32(18, 9, 36, 242), 128);
        draw->AddCircle(orb, orbR * 0.78f, ImGui::GetColorU32(ImVec4(limeAccent.x, limeAccent.y, limeAccent.z, 0.72f)), 128, S(2.0f));
        draw->AddCircle(orb, orbR * (0.28f + pulseCore * 0.035f), ImGui::GetColorU32(ImVec4(0.30f, 1.00f, 0.40f, 0.24f + pulseCore * 0.18f)), 96, S(1.4f));
        for (float y = orb.y - orbR * 0.58f; y <= orb.y + orbR * 0.58f; y += S(7.0f)) {
            const float span = std::sqrt(std::max(0.0f, orbR * orbR * 0.34f - (y - orb.y) * (y - orb.y))) * 1.65f;
            draw->AddLine(ImVec2(orb.x - span, y), ImVec2(orb.x + span, y), IM_COL32(178, 104, 255, 54), S(1.0f));
        }
        for (float x = orb.x - orbR * 0.55f; x <= orb.x + orbR * 0.55f; x += S(10.0f)) {
            const float span = std::sqrt(std::max(0.0f, orbR * orbR * 0.30f - (x - orb.x) * (x - orb.x))) * 1.55f;
            draw->AddLine(ImVec2(x, orb.y - span), ImVec2(x, orb.y + span), IM_COL32(178, 104, 255, 32), S(1.0f));
        }
        const float scanY = orb.y - orbR * 0.52f + std::fmod(animationTime * S(32.0f), orbR * 1.04f);
        const float scanSpan = std::sqrt(std::max(0.0f, orbR * orbR * 0.34f - (scanY - orb.y) * (scanY - orb.y))) * 1.65f;
        draw->AddLine(ImVec2(orb.x - scanSpan, scanY), ImVec2(orb.x + scanSpan, scanY), ImGui::GetColorU32(ImVec4(0.40f, 1.00f, 0.50f, 0.45f)), S(1.8f));
        for (int p = 0; p < 28; ++p) {
            const float orbit = orbR * (0.43f + (p % 4) * 0.09f);
            const float ang = animationTime * (0.32f + (p % 5) * 0.035f) + p * 2.39996f;
            const float flicker = 0.45f + 0.55f * std::sin(animationTime * 4.0f + p);
            draw->AddCircleFilled(ImVec2(orb.x + std::cos(ang) * orbit, orb.y + std::sin(ang) * orbit * 0.72f), S(1.0f + (p % 3) * 0.35f), ImGui::GetColorU32(ImVec4(0.32f, 1.00f, 0.42f, 0.20f + flicker * 0.38f)), 8);
        }
        const char* coreA = "A";
        const float coreASize = F50 ? S(70.0f) : titleFont->FontSize * 3.2f * uiScale;
        const ImVec2 coreAText = titleFont->CalcTextSizeA(coreASize, FLT_MAX, 0.0f, coreA);
        draw->AddText(titleFont, coreASize, ImVec2(orb.x - coreAText.x * 0.5f, orb.y - coreAText.y * 0.5f + S(1.5f)), ImGui::GetColorU32(ImVec4(limeAccent.x, limeAccent.y, limeAccent.z, 0.34f)), coreA);
        draw->AddText(titleFont, coreASize, ImVec2(orb.x - coreAText.x * 0.5f, orb.y - coreAText.y * 0.5f), IM_COL32(245, 235, 255, 255), coreA);

        auto statusFor = [&](float start, float end) -> int {
            if (progress >= end) return 2;
            if (progress >= start) return 1;
            return 0;
        };
        auto drawCutPanel = [&](const ImVec2& a, const ImVec2& b, float cut, ImU32 fill, ImU32 border, float thickness) {
            ImVec2 pts[8] = {
                ImVec2(a.x + cut, a.y), ImVec2(b.x - cut, a.y), ImVec2(b.x, a.y + cut), ImVec2(b.x, b.y - cut),
                ImVec2(b.x - cut, b.y), ImVec2(a.x + cut, b.y), ImVec2(a.x, b.y - cut), ImVec2(a.x, a.y + cut)
            };
            draw->AddConvexPolyFilled(pts, 8, fill);
            draw->AddPolyline(pts, 8, border, ImDrawFlags_Closed, thickness);
        };
        auto drawStatusIcon = [&](const ImVec2& cc, int idx, int state) {
            const ImU32 color = state == 0 ? IM_COL32(105, 106, 122, 175) : ImGui::GetColorU32(limeAccent);
            if (idx == 0) {
                draw->AddLine(ImVec2(cc.x - S(10.0f), cc.y), ImVec2(cc.x - S(3.0f), cc.y + S(8.0f)), color, S(4.0f));
                draw->AddLine(ImVec2(cc.x - S(3.0f), cc.y + S(8.0f)), ImVec2(cc.x + S(12.0f), cc.y - S(11.0f)), color, S(4.0f));
            } else if (idx == 1) {
                draw->AddRect(ImVec2(cc.x - S(10.0f), cc.y - S(10.0f)), ImVec2(cc.x + S(10.0f), cc.y + S(10.0f)), color, S(2.0f), 0, S(3.0f));
                draw->AddLine(ImVec2(cc.x - S(10.0f), cc.y - S(10.0f)), ImVec2(cc.x, cc.y - S(18.0f)), color, S(2.0f));
                draw->AddLine(ImVec2(cc.x + S(10.0f), cc.y - S(10.0f)), ImVec2(cc.x, cc.y - S(18.0f)), color, S(2.0f));
            } else if (idx == 2) {
                draw->AddLine(ImVec2(cc.x - S(13.0f), cc.y - S(7.0f)), ImVec2(cc.x + S(12.0f), cc.y - S(7.0f)), color, S(4.0f));
                draw->AddTriangleFilled(ImVec2(cc.x + S(12.0f), cc.y - S(14.0f)), ImVec2(cc.x + S(22.0f), cc.y - S(7.0f)), ImVec2(cc.x + S(12.0f), cc.y), color);
                draw->AddLine(ImVec2(cc.x + S(13.0f), cc.y + S(8.0f)), ImVec2(cc.x - S(12.0f), cc.y + S(8.0f)), color, S(4.0f));
                draw->AddTriangleFilled(ImVec2(cc.x - S(12.0f), cc.y + S(1.0f)), ImVec2(cc.x - S(22.0f), cc.y + S(8.0f)), ImVec2(cc.x - S(12.0f), cc.y + S(15.0f)), color);
            } else {
                draw->AddRect(ImVec2(cc.x - S(10.0f), cc.y - S(1.0f)), ImVec2(cc.x + S(10.0f), cc.y + S(13.0f)), color, S(2.0f), 0, S(2.0f));
                draw->AddBezierCubic(ImVec2(cc.x - S(8.0f), cc.y - S(1.0f)), ImVec2(cc.x - S(8.0f), cc.y - S(17.0f)), ImVec2(cc.x + S(8.0f), cc.y - S(17.0f)), ImVec2(cc.x + S(8.0f), cc.y - S(1.0f)), color, S(2.0f), 18);
            }
        };
        auto drawStatus = [&](int idx, const char* title, const char* desc, int state, float localProgress) {
            const float x = innerMin.x + innerW * 0.52f;
            const float cardH = S(86.0f);
            const float cardGap = S(13.0f);
            const float y = innerMin.y + S(78.0f) + idx * (cardH + cardGap);
            const float w = innerW * 0.43f;
            const float h = cardH;
            ImVec2 a(x, y), b(x + w, y + h);
            const ImU32 fill = state == 0 ? IM_COL32(9, 9, 14, 130) : IM_COL32(13, 13, 20, 220);
            const ImU32 border = state == 1 ? ImGui::GetColorU32(limeAccent) : IM_COL32(112, 115, 132, state == 0 ? 70 : 135);
            if (state == 1) {
                for (int glow = 3; glow >= 1; --glow) {
                    drawCutPanel(ImVec2(a.x - S(glow * 2.0f), a.y - S(glow * 2.0f)), ImVec2(b.x + S(glow * 2.0f), b.y + S(glow * 2.0f)), S(14.0f), IM_COL32(0, 0, 0, 0), ImGui::GetColorU32(ImVec4(limeAccent.x, limeAccent.y, limeAccent.z, 0.08f * glow)), S(1.0f + glow));
                }
            }
            drawCutPanel(a, b, S(14.0f), fill, border, state == 1 ? S(2.0f) : S(1.0f));
            ImVec2 cc(a.x + S(42.0f), a.y + h * 0.5f);
            draw->AddCircle(cc, S(22.0f), state == 0 ? IM_COL32(92, 94, 110, 120) : ImGui::GetColorU32(limeAccent), 36, S(2.0f));
            drawStatusIcon(cc, idx, state);
            draw->AddText(labelFont, bodySize * 0.88f, ImVec2(a.x + S(78.0f), a.y + S(21.0f)), state == 0 ? IM_COL32(120, 122, 140, 170) : IM_COL32(245, 245, 250, 250), title);
            draw->AddText(labelFont, labelSize * 0.80f, ImVec2(a.x + S(78.0f), a.y + S(45.0f)), state == 0 ? IM_COL32(105, 108, 126, 150) : IM_COL32(198, 200, 214, 230), desc);
            const char* stateText = state == 2 ? "COMPLETE" : (state == 1 ? "IN PROGRESS" : "PENDING");
            const float stateSize = labelSize * 0.72f;
            ImVec2 ts = labelFont->CalcTextSizeA(stateSize, FLT_MAX, 0.0f, stateText);
            draw->AddText(labelFont, stateSize, ImVec2(b.x - ts.x - S(38.0f), a.y + h * 0.5f - S(6.0f)), state == 0 ? IM_COL32(130, 132, 148, 170) : ImGui::GetColorU32(limeAccent), stateText);
            draw->AddCircle(ImVec2(b.x - S(22.0f), a.y + h * 0.5f), S(10.0f), state == 0 ? IM_COL32(120, 122, 138, 150) : ImGui::GetColorU32(limeAccent), 24, S(2.0f));
            if (state == 2) {
                draw->AddLine(ImVec2(b.x - S(27.0f), a.y + h * 0.5f), ImVec2(b.x - S(23.0f), a.y + h * 0.5f + S(5.0f)), ImGui::GetColorU32(limeAccent), S(2.0f));
                draw->AddLine(ImVec2(b.x - S(23.0f), a.y + h * 0.5f + S(5.0f)), ImVec2(b.x - S(16.0f), a.y + h * 0.5f - S(6.0f)), ImGui::GetColorU32(limeAccent), S(2.0f));
            } else if (state == 1) {
                const ImVec2 spin(b.x - S(22.0f), a.y + h * 0.5f);
                for (int s = 0; s < 10; ++s) {
                    const float alpha = (s + 1) / 10.0f;
                    const float ang = animationTime * 5.0f + s * 0.6283185f;
                    draw->AddCircleFilled(ImVec2(spin.x + std::cos(ang) * S(13.0f), spin.y + std::sin(ang) * S(13.0f)), S(2.0f), ImGui::GetColorU32(ImVec4(limeAccent.x, limeAccent.y, limeAccent.z, 0.10f + alpha * 0.42f)), 8);
                }
            }
            if (idx == 2) {
                const float barX = a.x + S(34.0f);
                const float barY = b.y - S(15.0f);
                draw->AddRectFilled(ImVec2(barX, barY), ImVec2(b.x - S(74.0f), barY + S(5.0f)), IM_COL32(0, 0, 0, 210), S(3.0f));
                draw->AddRectFilled(ImVec2(barX, barY), ImVec2(barX + (b.x - S(74.0f) - barX) * localProgress, barY + S(5.0f)), ImGui::GetColorU32(limeAccent), S(3.0f));
                char pct[16]; std::snprintf(pct, sizeof(pct), "%d%%", (int)std::lround(progress * 100.0f));
                draw->AddText(labelFont, bodySize * 0.84f, ImVec2(b.x - S(50.0f), barY - S(6.0f)), IM_COL32(255, 255, 255, 245), pct);
            }
        };
        drawStatus(0, "SESSION VALIDATION", "Verifying secure credentials", statusFor(0.0f, 0.20f), 1.0f);
        drawStatus(1, "CONFIGURATION LOAD", "Loading system parameters", statusFor(0.20f, 0.35f), 1.0f);
        drawStatus(2, "INTERFACE HANDOFF", "Transitioning to secure channel", statusFor(0.35f, 0.90f), ImClamp((progress - 0.35f) / 0.55f, 0.0f, 1.0f));
        drawStatus(3, "SECURITY INITIATION", "Establishing encrypted link", statusFor(0.90f, 1.0f), 0.0f);

        const ImVec2 barMin(innerMin.x + S(44.0f), innerMax.y - S(90.0f));
        const ImVec2 barMax(innerMax.x - S(44.0f), innerMax.y - S(30.0f));

        const float detailX = innerMin.x + innerW * 0.52f;
        const float detailW = innerW * 0.43f;
        const float detailY = innerMin.y + S(78.0f) + 4.0f * S(86.0f) + 3.0f * S(13.0f) + S(18.0f);
        const float detailH = std::max(S(64.0f), barMin.y - detailY - S(14.0f));
        if (detailH > S(34.0f)) {
            drawCutPanel(
                ImVec2(detailX, detailY),
                ImVec2(detailX + detailW, detailY + detailH),
                S(10.0f),
                IM_COL32(8, 8, 13, 215),
                IM_COL32(91, 78, 122, 95),
                S(1.0f)
            );
            draw->AddText(labelFont, labelSize * 0.66f, ImVec2(detailX + S(14.0f), detailY + S(9.0f)), IM_COL32(175, 169, 205, 220), "SECURE CHANNEL");
            const char* detailMode = "SYNC ACTIVE";
            ImVec2 detailModeSize = labelFont->CalcTextSizeA(labelSize * 0.62f, FLT_MAX, 0.0f, detailMode);
            draw->AddText(labelFont, labelSize * 0.62f, ImVec2(detailX + detailW - detailModeSize.x - S(14.0f), detailY + S(9.0f)), ImGui::GetColorU32(limeAccent), detailMode);
            const float chipGap = S(8.0f);
            const float chipW = (detailW - S(28.0f) - chipGap * 2.0f) / 3.0f;
            auto detailChip = [&](int idx, const char* label, const char* value, float phase) {
                const ImVec2 a(detailX + S(14.0f) + idx * (chipW + chipGap), detailY + S(30.0f));
                const ImVec2 b(a.x + chipW, detailY + detailH - S(12.0f));
                draw->AddRectFilled(a, b, IM_COL32(18, 16, 25, 185), S(5.0f));
                draw->AddRect(a, b, IM_COL32(116, 84, 166, 105), S(5.0f), 0, S(1.0f));
                const ImVec2 dot(a.x + S(12.0f), a.y + S(13.0f));
                const float pulse = 0.55f + 0.45f * std::sin(animationTime * 2.8f + phase);
                draw->AddCircleFilled(dot, S(3.5f), ImGui::GetColorU32(ImVec4(limeAccent.x, limeAccent.y, limeAccent.z, 0.55f + pulse * 0.35f)), 12);
                draw->AddText(labelFont, labelSize * 0.56f, ImVec2(a.x + S(23.0f), a.y + S(7.0f)), IM_COL32(145, 146, 166, 220), label);
                draw->AddText(labelFont, bodySize * 0.62f, ImVec2(a.x + S(10.0f), a.y + S(24.0f)), IM_COL32(238, 236, 248, 238), value);
                const float railY = b.y - S(10.0f);
                draw->AddRectFilled(ImVec2(a.x + S(10.0f), railY), ImVec2(b.x - S(10.0f), railY + S(3.0f)), IM_COL32(30, 28, 39, 255), S(2.0f));
                draw->AddRectFilled(ImVec2(a.x + S(10.0f), railY), ImVec2(a.x + S(10.0f) + (chipW - S(20.0f)) * (0.45f + 0.25f * pulse), railY + S(3.0f)), ImGui::GetColorU32(limeAccent), S(2.0f));
            };
            detailChip(0, "NODE", "DARMODZ V5", 0.0f);
            detailChip(1, "PING", "14 MS", 1.2f);
            detailChip(2, "PACKETS", "4096", 2.4f);
        }

        draw->AddRectFilled(barMin, barMax, IM_COL32(9, 9, 15, 245), S(8.0f));
        draw->AddRect(barMin, barMax, IM_COL32(88, 72, 126, 95), S(8.0f));
        const float footerW = barMax.x - barMin.x;
        const float footerSlot = footerW * 0.25f;
        auto footer = [&](int col, int iconType, const char* a, const char* b) {
            const float x = barMin.x + footerSlot * col + S(24.0f);
            draw->AddCircle(ImVec2(x, barMin.y + S(30.0f)), S(14.0f), ImGui::GetColorU32(limeAccent), 24, S(1.8f));
            const ImVec2 fc(x, barMin.y + S(30.0f));
            const ImU32 footerAccent = ImGui::GetColorU32(limeAccent);
            if (iconType == 0) {
                draw->AddLine(ImVec2(fc.x - S(7.0f), fc.y + S(7.0f)), ImVec2(fc.x, fc.y - S(8.0f)), footerAccent, S(2.4f));
                draw->AddLine(ImVec2(fc.x, fc.y - S(8.0f)), ImVec2(fc.x + S(7.0f), fc.y + S(7.0f)), footerAccent, S(2.4f));
                draw->AddLine(ImVec2(fc.x - S(2.5f), fc.y + S(2.0f)), ImVec2(fc.x + S(3.5f), fc.y + S(7.0f)), footerAccent, S(2.4f));
            } else if (iconType == 1) {
                for (int i = 0; i < 4; ++i) {
                    const float h = S(4.0f + i * 3.5f);
                    draw->AddRectFilled(ImVec2(fc.x - S(9.0f) + i * S(5.5f), fc.y + S(8.0f) - h), ImVec2(fc.x - S(5.5f) + i * S(5.5f), fc.y + S(8.0f)), footerAccent, S(1.0f));
                }
            } else if (iconType == 2) {
                draw->AddTriangle(ImVec2(fc.x, fc.y - S(10.0f)), ImVec2(fc.x - S(9.0f), fc.y - S(3.5f)), ImVec2(fc.x + S(9.0f), fc.y - S(3.5f)), footerAccent, S(1.8f));
                draw->AddLine(ImVec2(fc.x - S(9.0f), fc.y - S(3.5f)), ImVec2(fc.x - S(5.5f), fc.y + S(9.0f)), footerAccent, S(1.8f));
                draw->AddLine(ImVec2(fc.x + S(9.0f), fc.y - S(3.5f)), ImVec2(fc.x + S(5.5f), fc.y + S(9.0f)), footerAccent, S(1.8f));
                draw->AddLine(ImVec2(fc.x - S(5.5f), fc.y + S(9.0f)), ImVec2(fc.x + S(5.5f), fc.y + S(9.0f)), footerAccent, S(1.8f));
            } else {
                draw->AddCircle(fc, S(7.0f), footerAccent, 18, S(1.8f));
                draw->AddLine(fc, ImVec2(fc.x, fc.y - S(5.5f)), footerAccent, S(1.8f));
                draw->AddLine(fc, ImVec2(fc.x + S(4.5f), fc.y + S(2.5f)), footerAccent, S(1.8f));
            }
            draw->AddText(labelFont, labelSize * 0.70f, ImVec2(x + S(30.0f), barMin.y + S(16.0f)), IM_COL32(196, 198, 214, 230), a);
            draw->AddText(labelFont, bodySize * 0.68f, ImVec2(x + S(30.0f), barMin.y + S(35.0f)), IM_COL32(255, 255, 255, 242), b);
        };
        footer(0, 0, "SYSTEM STATUS", "SECURE CONNECTION");
        footer(1, 1, "NETWORK", "ENCRYPTED");
        footer(2, 2, "PROTOCOL", "DARMODZ V5");
        footer(3, 3, "EST. TIME REMAINING", "00:02:34");
    }
    ImGui::End();

    loadingAnimationTimer += io.DeltaTime;
    if (loadingAnimationTimer >= loadingAnimationDuration) {
        showLoadingAnimation = false;
        loadingAnimationTimer = 0.0f;
        finished = true;
    }

    return finished;
}

}