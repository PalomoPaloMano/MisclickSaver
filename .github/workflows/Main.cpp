#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/binding/CCMenuItemToggler.hpp>

using namespace geode::prelude;

bool g_enabled = true;
bool g_showMessage = true;

class $modify(MisclickSaverPlayLayer, PlayLayer) {

    void destroyPlayer(PlayerObject* player, GameObject* object) {
        // Si el mod está desactivado → muerte normal
        if (!g_enabled) {
            return PlayLayer::destroyPlayer(player, object);
        }

        // === LO QUE PEDISTE ===
        // En vez de morir, lo tratamos como que lo hiciste bien
        // No llamamos a la función original → no mueres

        if (player) {
            // Pequeño ajuste para que no se sienta raro al salvar
            if (player->m_yVelocity < 0) {
                player->m_yVelocity = std::max(player->m_yVelocity, -2.0f);
            }
        }

        // Mensaje de que lo hiciste bien
        if (g_showMessage && player) {
            auto label = CCLabelBMFont::create("¡Lo hiciste bien!", "bigFont.fnt");
            label->setScale(0.55f);
            label->setColor({80, 255, 120});
            label->setPosition(player->getPosition() + ccp(0, 45));
            label->setOpacity(255);
            this->addChild(label, 9999);

            label->runAction(CCSequence::create(
                CCDelayTime::create(0.75f),
                CCFadeOut::create(0.4f),
                CCRemoveSelf::create(),
                nullptr
            ));
        }
    }
};

class $modify(MisclickSaverPauseLayer, PauseLayer) {

    void customSetup() {
        PauseLayer::customSetup();

        // Crear el botón de toggle
        auto onSprite = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
        auto offSprite = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");

        auto toggler = CCMenuItemToggler::create(
            offSprite,
            onSprite,
            this,
            menu_selector(MisclickSaverPauseLayer::onToggleMisclick)
        );

        // Estado inicial del toggle
        toggler->toggle(g_enabled);

        // Texto al lado
        auto label = CCLabelBMFont::create("Misclick Saver", "bigFont.fnt");
        label->setScale(0.35f);
        label->setAnchorPoint({0.f, 0.5f});

        // Menú
        auto menu = CCMenu::create();
        menu->setPosition({0, 0});

        // Posición (arriba a la derecha del menú de pausa)
        auto winSize = CCDirector::get()->getWinSize();
        toggler->setPosition({winSize.width - 40.f, winSize.height - 40.f});
        label->setPosition({winSize.width - 150.f, winSize.height - 40.f});

        menu->addChild(toggler);
        menu->addChild(label);

        this->addChild(menu, 100);
    }

    void onToggleMisclick(CCObject* sender) {
        g_enabled = !g_enabled;
        Mod::get()->setSettingValue("enabled", g_enabled);

        // Feedback rápido
        auto text = g_enabled ? "Misclick Saver: ON" : "Misclick Saver: OFF";
        auto notif = CCLabelBMFont::create(text, "bigFont.fnt");
        notif->setScale(0.4f);
        notif->setColor(g_enabled ? ccc3(100, 255, 100) : ccc3(255, 90, 90));

        auto winSize = CCDirector::get()->getWinSize();
        notif->setPosition({winSize.width / 2, winSize.height - 70});
        this->addChild(notif, 999);

        notif->runAction(CCSequence::create(
            CCDelayTime::create(1.0f),
            CCFadeOut::create(0.4f),
            CCRemoveSelf::create(),
            nullptr
        ));
    }
};

$on_mod(Loaded) {
    g_enabled = Mod::get()->getSettingValue<bool>("enabled");
    g_showMessage = Mod::get()->getSettingValue<bool>("show-message");

    listenForSettingChanges("enabled", [](bool v) {
        g_enabled = v;
    });
    listenForSettingChanges("show-message", [](bool v) {
        g_showMessage = v;
    });
}
