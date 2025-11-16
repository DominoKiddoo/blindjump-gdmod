#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

bool disableblind = false;
std::string covercolour = "Black";
bool foMode = false;
double foduration = 1.0;
bool fbMode = false;
double fbduration = 1.0;
bool fbsound = false;

$on_mod(Loaded) {
	auto mod = Mod::get();

	disableblind = mod->getSettingValue<bool>("disableb");
	listenForSettingChangesV3<bool>("disableb", [](bool value) {
		disableblind = value;
	}, mod);

	covercolour = mod->getSettingValue<std::string>("jccolour");
	listenForSettingChangesV3<std::string>("jccolour", [](std::string const& value) {
		covercolour = value;
	}, mod);

	foMode = mod->getSettingValue<bool>("fomode");
	listenForSettingChangesV3<bool>("fomode", [](bool value) {
		foMode = value;
	}, mod);

	foduration = mod->getSettingValue<double>("foduration");
	listenForSettingChangesV3<double>("foduration", [](double value) {
		foduration = value;
	}, mod);

	fbMode = mod->getSettingValue<bool>("flashbangmode");
	listenForSettingChangesV3<bool>("flashbangmode", [](bool value) {
		fbMode = value;
	}, mod);

	fbduration = mod->getSettingValue<double>("flashbangduration");
	listenForSettingChangesV3<double>("flashbangduration", [](double value) {
		fbduration = value;
	}, mod);

	fbsound = mod->getSettingValue<bool>("fbsound");
	listenForSettingChangesV3<bool>("fbsound", [](bool value) {
		fbsound = value;
	}, mod);
}

class $modify(BJLayer, PlayLayer) {
	struct Fields {
		CCSprite* jcover = nullptr;
	};

	bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
		if (!PlayLayer::init(level, useReplay, dontCreateObjects))
			return false;

		log::info("loaded for level {}", level->m_levelName);

		auto covercolour = Mod::get()->getSettingValue<std::string>("jccolour");
		log::info("Cover colour: {}", covercolour);

		CCSprite* jcover = nullptr;

		if (covercolour == "Black") {
			jcover = CCSprite::create("Black.png"_spr);
		} else if (covercolour == "White") {
			jcover = CCSprite::create("White.png"_spr);
		} else if (covercolour == "Soggy") {
			jcover = CCSprite::create("Soggy.png"_spr);
		} else if (covercolour == "Chinchilla") {
			jcover = CCSprite::create("Chinchilla.png"_spr);
		} else {
			log::error("Sprite failed.");
			return true;
		}

		if (jcover) {
			auto winSize = CCDirector::sharedDirector()->getWinSize();
			jcover->setZOrder(1000);

			auto jcSize = jcover->getContentSize();
			float scaleX = winSize.width / jcSize.width;
			float scaleY = winSize.height / jcSize.height;

			jcover->setScaleX(scaleX);
			jcover->setScaleY(scaleY);

			jcover->setPosition({ winSize.width / 2, winSize.height / 2 });
			jcover->setID("jcover"_spr);
			this->addChild(jcover);
			jcover->setVisible(false);

			this->m_fields->jcover = jcover;
			log::info("Sprite added!");
		} else {
			log::error("Failed to load sprite!");
			return true;
		}
		return true;
	}

	void destroyPlayer(PlayerObject* player, GameObject* cause) {
		if (cause != m_anticheatSpike) {
			m_fields->jcover->setVisible(false);
		}
		PlayLayer::destroyPlayer(player, cause);
	}
};

#include <Geode/modify/GJBaseGameLayer.hpp>

class $modify(BJHookLayer, GJBaseGameLayer) {
	void handleButton(bool down, int button, bool isPlayer1) {
		GJBaseGameLayer::handleButton(down, button, isPlayer1);

		if (m_isEditor || disableblind) {
			return;
		}

		if (auto jcover = reinterpret_cast<BJLayer*>(this)->m_fields->jcover) {
			if (down) {
				if (fbsound) {
					FMODAudioEngine::sharedEngine()->playEffect("flashbang.mp3"_spr);
				}
				if (foMode) {
					jcover->stopAllActions();
					resetforfomode();
					jcover->setVisible(true);

				}
				jcover->setVisible(true);
				// log::info("fb duration: {}", fbduration);
				if (fbMode) {
					jcover->stopAllActions();
					jcover->runAction(
						CCSequence::create(
							CCDelayTime::create(0.0f),
							CCFadeOut::create(fbduration),
							CCCallFunc::create(this, callfunc_selector(BJHookLayer::resetjcover)),
							nullptr
						)
					);
				}
			} else {
				if (!fbMode) {
					if (foMode) {
						// jcover->stopAllActions();
						jcover->runAction(
							CCSequence::create(
								CCDelayTime::create(0.0f),
								CCFadeOut::create(foduration),
								CCCallFunc::create(this, callfunc_selector(BJHookLayer::resetjcover)),
								nullptr
						));
					} else {
						jcover->setVisible(false);
					}
				}
			}
		}
	}

	void resetjcover() {
		if (auto jcover = reinterpret_cast<BJLayer*>(this)->m_fields->jcover) {
			jcover->setOpacity(0);
			jcover->setVisible(false);
		}
	}
	
	void resetforfomode() {
		if (auto jcover = reinterpret_cast<BJLayer*>(this)->m_fields->jcover) {
			jcover->setOpacity(255);
		}
	}
};
