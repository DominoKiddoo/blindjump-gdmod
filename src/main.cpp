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
bool bipm = true;
int minp = 0;
int maxp = 100;
int bchance = 100;
bool urinplatformer = true;

std::string bmode = "Normal";
std::string pmode = "Never";


$on_mod(Loaded) {
	auto mod = Mod::get();
	std::srand(std::time(nullptr));

	disableblind = mod->getSettingValue<bool>("disableb");
	listenForSettingChangesV3<bool>("disableb", [](bool value) {
		disableblind = value;
	}, mod);

	covercolour = mod->getSettingValue<std::string>("jccolour");
	listenForSettingChangesV3<std::string>("jccolour", [](std::string const& value) {
		covercolour = value;
	}, mod);

	foduration = mod->getSettingValue<double>("foduration");
	listenForSettingChangesV3<double>("foduration", [](double value) {
		foduration = value;
	}, mod);

	fbduration = mod->getSettingValue<double>("flashbangduration");
	listenForSettingChangesV3<double>("flashbangduration", [](double value) {
		fbduration = value;
	}, mod);

	fbsound = mod->getSettingValue<bool>("fbsound");
	listenForSettingChangesV3<bool>("fbsound", [](bool value) {
		fbsound = value;
	}, mod);

	bmode = mod->getSettingValue<std::string>("bmode");
	listenForSettingChangesV3<std::string>("bmode", [](std::string value) {
		bmode = value;
		
	}, mod);

	bipm = mod->getSettingValue<bool>("buttoninp");
	listenForSettingChangesV3<bool>("buttoninp", [](bool value) {
		bipm = value;
	}, mod);

	minp = mod->getSettingValue<int>("minp");
	listenForSettingChangesV3<int>("minp", [](int value) {
		minp = value;
	}, mod);

	maxp = mod->getSettingValue<int>("maxp");
	listenForSettingChangesV3<int>("maxp", [](int value) {
		maxp = value;
	}, mod);

	bchance = mod->getSettingValue<int>("bchance");
	listenForSettingChangesV3<int>("bchance", [](int value) {
		bchance = value;
	}, mod);

	pmode = mod->getSettingValue<std::string>("pmode");
	listenForSettingChangesV3<std::string>("pmode", [](std::string value) {
		pmode = value;

		if (value != "Never") {
			FLAlertLayer::create("Blind Jumps", "Setting this to a value other than 'Never' is buggy and not recommended", "OK")->show();
		}
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
		urinplatformer = level->isPlatformer();
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
		} else if (covercolour == "Custom") {
			auto imagePath = Mod::get()->getSettingValue<std::filesystem::path>("cimage");
			jcover = CCSprite::create(geode::utils::string::pathToString(imagePath).c_str());
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
		if (this == nullptr) {
			log::error("skipping safely");
			return;
		}

		if (!m_fields->jcover) {
			log::warn("skipping jcover access");
			PlayLayer::destroyPlayer(player, cause);
			return;
		}

		if (cause != m_anticheatSpike) {
			if (m_fields->jcover) {
				// safe to access
				m_fields->jcover->setVisible(false);
			} else {
				log::warn("jcover is null");
			}
		}

		// call base implementation last
		PlayLayer::destroyPlayer(player, cause);
	}
};

#include <Geode/modify/GJBaseGameLayer.hpp>
#include <cstdlib>
#include <ctime>


class $modify(BJHookLayer, GJBaseGameLayer) {
	void handleButton(bool down, int button, bool isPlayer1) {
		GJBaseGameLayer::handleButton(down, button, isPlayer1);



		if (m_isEditor || disableblind) {
			return;
		}

		auto jcover = reinterpret_cast<BJLayer*>(this)->m_fields->jcover;
		if (!jcover) return;

		auto imagePath = Mod::get()->getSettingValue<std::filesystem::path>("cimage");
		
		auto tex = CCTextureCache::sharedTextureCache()->addImage(
				geode::utils::string::pathToString(imagePath).c_str(),
				false
		);


		if (tex) {
				jcover->setTexture(tex);
				jcover->setTextureRect({
						0,
						0,
						tex->getContentSize().width,
						tex->getContentSize().height
				});

			auto winSize = CCDirector::sharedDirector()->getWinSize();

			float scaleX = winSize.width / tex->getContentSize().width;
			float scaleY = winSize.height / tex->getContentSize().height;

			jcover->setScaleX(scaleX);
			jcover->setScaleY(scaleY);
		}




		if (down) {
			// get %
			float percent = 0.f;
			if (urinplatformer) {
				if (pmode == "Never") {
					return;
				} else if (pmode == "Only jumping" && (button == 2 || button == 3)) {
					return;
				}
			}

			if (!urinplatformer && (button == 2 || button == 3)) {
				return;
			}
			if (auto pl = PlayLayer::get()) {
				percent = pl->getCurrentPercent();
			}



			if (percent < minp || percent > maxp) {
				return;
			}

			float chancePercent = bchance;

			float roll = static_cast<float>(std::rand()) / RAND_MAX * 100.0f;

			if (roll >= chancePercent) {
				return;
			}

			if (fbsound) {
				FMODAudioEngine::sharedState()->playEffect("flashbang.mp3"_spr);
			}
			if (bmode == "Fade-out") { 
				jcover->stopAllActions();
				resetforfomode();
				jcover->setVisible(true);

			}
			jcover->setVisible(true);
			if (bmode == "Flashbang") { 
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
			if (bmode != "Flashbang") {
				if (bmode == "Fade-out") {
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

#include <Geode/modify/PauseLayer.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>

class $modify(BJButtonLayer, PauseLayer) {
	void customSetup() {
		PauseLayer::customSetup();

		auto menu = this->getChildByID("left-button-menu");
		auto icon = CCSprite::create("mlogo.png"_spr);
		icon->setScale(0.5f);

		auto circleSpr = CircleButtonSprite::create(
			icon,
			CircleBaseColor::Green,
			CircleBaseSize::Medium
		);

		auto bjsettingsbutton = CCMenuItemSpriteExtra::create(
			circleSpr,
			this,
			menu_selector(BJButtonLayer::onbjsettingsbutton)
		);

		if (bipm) {
			bjsettingsbutton->setID("bjsettingsbutton");
			menu->addChild(bjsettingsbutton);
			menu->updateLayout();
		}
	}

	void onbjsettingsbutton(CCObject*) {
		geode::openSettingsPopup(Mod::get(), true);
	}
};
