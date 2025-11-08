#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(BJLayer, PlayLayer) {
	bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
		if (!PlayLayer::init(level, useReplay, dontCreateObjects))
			return false;

		log::info("loaded for level {}", level->m_levelName);


		auto covercolour = Mod::get()->getSettingValue<std::string>("jccolour");
		log::info("Cover colour: {}", covercolour);

		CCSprite* jcover = nullptr;

		if (covercolour == "Black") {
			jcover = CCSprite::create("Black.png"_spr);	
		} else if (covercolour == "White")
		{
			jcover = CCSprite::create("White.png"_spr);

		} else if (covercolour == "Soggy")
		{
			jcover = CCSprite::create("Soggy.png"_spr);
		} else {
			log::error("Sprite failed.");
			return false;
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
			jcover->setTag(123456789987654321);    
			this->addChild(jcover);
			
			jcover->setVisible(false);
			log::info("Sprite added!");
		} else {
			log::error("Failed to load sprite!");
		}




		return true;
	}

	void destroyPlayer(PlayerObject* player, GameObject* cause) {

		if (cause != m_anticheatSpike) {
			auto jcover = this->getChildByTag(123456789987654321);
			jcover->setVisible(false);

		}
		PlayLayer::destroyPlayer(player, cause);
		

	}

	
};


#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;



class $modify(BJHookLayer, GJBaseGameLayer) {
	void handleButton(bool down, int button, bool isPlayer1) {
		auto disableblind = Mod::get()->getSettingValue<bool>("disableb");
		GJBaseGameLayer::handleButton(down, button, isPlayer1);
		if (disableblind == false) {


			
			auto jcover = getChildByTag(123456789987654321);
			auto fbMode = Mod::get()->getSettingValue<bool>("flashbangmode");
			auto foMode = Mod::get()->getSettingValue<bool>("fomode");
			auto foduration = Mod::get()->getSettingValue<double>("foduration");
			if (down) {
				auto fbsound = Mod::get()->getSettingValue<bool>("fbsound");
				if (fbsound == true) {
					FMODAudioEngine::sharedEngine()->playEffect("flashbang.mp3"_spr);
				}
				




				if (foMode == true) {
					jcover->stopAllActions();
					resetforfomode();
					jcover->setVisible(true);

				}
				jcover->setVisible(true);
				auto fbduration = Mod::get()->getSettingValue<double>("flashbangduration");
				
				// log::info("fb duration: {}", fbduration);
				if (fbMode == true) {
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
				if (fbMode != true) {
					if (foMode == true) {
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
		if (auto jcover = getChildByTag(123456789987654321)) {
			if (auto rgbaNode = typeinfo_cast<cocos2d::CCRGBAProtocol*>(jcover)) {
				rgbaNode->setOpacity(0);
			}
			jcover->setVisible(false);
		}
	}
	
	void resetforfomode() {
		if (auto jcover = getChildByTag(123456789987654321)) {
			if (auto rgbaNode = typeinfo_cast<cocos2d::CCRGBAProtocol*>(jcover)) {
				rgbaNode->setOpacity(255);
			}
		}
	}
};



