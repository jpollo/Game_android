/*
 * GameScene.cpp
 *
 *  Created on: 2012-9-17
 *      Author: arch
 */

#include "MenuScene.h"
#include "GameScene.h"

#include "SimpleAudioEngine.h"
#include "Contact/MyContactListener.h"
#include "Utilty/UILayer.h"
#include "Config/GameConstants.h"
#include <list>
#include <vector>


using namespace cocos2d;
using namespace CocosDenshion;


#define TILE_SIZE 32
#define PT_RATIO 32

GameScene::GameScene() {
	contactListener = new MyContactListener();
	//TODO new word
	b2Vec2 gravity(0, 0);
	world = new b2World(gravity);

}

GameScene::~GameScene() {
	if (world) {
		delete world;
	}
}

CCScene* GameScene::scene() {
	CCScene *scene = CCScene::create();

	GameScene *layer = GameScene::create();

	scene->addChild(layer);

	return scene;
}

bool GameScene::init() {
	if (!CCLayer::init()) {
		return false;
	}

	CCSize size = CCDirector::sharedDirector()->getWinSize();
	//720 1184
	//添加背景
	backSprite1 = CCSprite::create("back1.jpg");
	backSprite2 = CCSprite::create("back1.jpg");
	backSprite1->setAnchorPoint(ccp(0, 0));
	backSprite2->setAnchorPoint(ccp(0, 0));
	backSprite1->setPosition(ccp(0, 0));
	backSprite2->setPosition(ccp(0, size.height));
	this->addChild(backSprite1, 0, BACK_LAYER1);
	this->addChild(backSprite2, 0, BACK_LAYER2);

	//TODO 添加血条
	UILayer* ui = new UILayer();
	this->addChild(ui, 0);
	//TODO 没周期更新函数
	//TODO 判断reduce 判断collision
	/**
	 *
if(! isreduce && iscollision(gameplayer,enemy)){
  CCActionInterval*  action = CCBlink::actionWithDuration(5, 10);
  gameplayer->runAction(action);
  schedule(schedule_selector(MapScene::resetreduce), 5.0f);
  isreduce = true;
  hmove = 0;
  ui->setlife(ui->getlife() - 5);
}
	 *
	 */


	//TODO 添加life or 子弹库（并排图片）

	//添加精灵
	plane = CCSprite::create("plane.png");
	plane->setPosition(ccp(60,60));
	this->addChild(plane, 2, TAG_HERO);


	//添加返回按钮
	CCMenuItemFont *backMenu = CCMenuItemFont::create("Back", this, menu_selector(GameScene::menuBackCallback));
	//exit->setAnchorPoint(CCPointZero);
	backMenu->setPosition(ccp(size.width/2,size.height/2));

	CCMenu* pMenu = CCMenu::create(backMenu, NULL);
	pMenu->setPosition(CCPointZero);
	this->addChild(pMenu, 1);

	//添加敌方部队
	//schedule_selector
	//每帧调用

	//添加粒子系统
//	this->addParticle();
	//---碰撞检测
	// create physic world
//	b2Vec2 gravity(0, 0);
//	world = new b2World(gravity);
	world->SetAllowSleeping(false);
	//contact listener

	this->contactListener = new MyContactListener();
	world->SetContactListener(contactListener);

//	  spawnCar();
//
	schedule(schedule_selector(GameScene::updateBackGround));
	schedule(schedule_selector(GameScene::tick));
	schedule(schedule_selector(GameScene::gameLogic), 1.0);
	schedule(schedule_selector(GameScene::update), 1.f);
	//更新子弹
//	schedule(schedule_selector(GameScene::shootBullet), 1.0);

	//TODO import 不然触屏无法使用
	this->setTouchEnabled(true);

	return true;
}

//退出所有程序
void GameScene::menuCloseCallback(CCObject* pSender) {
	CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

void GameScene::menuBackCallback(CCObject* pSender) {
	CCDirector::sharedDirector()->replaceScene(MenuScene::scene());
}

//注册
void GameScene::registerWithTouchDispatcher() {
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
}

bool GameScene::ccTouchBegan(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) {
	//获取触点坐标
	CCPoint touchLocation = touch->getLocationInView();
	touchLocation = CCDirector::sharedDirector()->convertToGL(touchLocation);

	m_tBeginPos = touchLocation; //new m_tBeginPos;
	//获取Hero TODO 用tag获取
	CCPoint heroLocation = plane->getPosition();
	CCSize heroSize = plane->getContentSize();

//	if((touchLocation.x>heroLocation.x-heroSize.width/2) && (touchLocation.x<heroLocation.x+heroSize.width/2)
//			&& touchLocation.y>heroLocation.y && touchLocation.y<heroLocation.y+heroSize.height) {
//	if(true){
////		CCLog("满足条件");
//		stepIndex = -5;
//		xdelta = touchLocation.x - heroLocation.x;
//		ydelta = touchLocation.y - heroLocation.y;
//		if(abs(xdelta)>SPEED_HERO_X) {
//			xdelta = (xdelta/abs(xdelta))*SPEED_HERO_X;
//		}
//		if(abs(ydelta)>SPEED_HERO_Y) {
//			ydelta = (ydelta/abs(ydelta))*SPEED_HERO_Y;
//		}
//		CCLog("xdelta : %f", xdelta);
//		CCLog("ydelta : %f", ydelta);
//	}
	return true;
}

void GameScene::ccTouchMoved(cocos2d::CCTouch *touch, cocos2d::CCEvent * event) {
	//手指的位置
	CCPoint touchLocation = touch->getLocationInView();
//	CCPoint touchLocation = touch->getPreviousLocationInView();
	touchLocation = CCDirector::sharedDirector()->convertToGL(touchLocation);

	CCNode* heroNode = getChildByTag(TAG_HERO);
	CCPoint heroPoint = heroNode->getPosition();
	CCPoint movePos = ccpSub(touchLocation, m_tBeginPos);
	CCPoint nextPos = ccpAdd(heroPoint, movePos);
	if(nextPos.x<MAX_X&&nextPos.x>MIN_X && nextPos.y<MAX_Y&& nextPos.y>MIN_Y) {
		heroNode->setPosition(nextPos);
	}
	m_tBeginPos = touchLocation;
	//sprite -> setPosition
//	CCNode* pNode = getChildByTag(1000);
//	if (pNode != NULL) {
//		pNode->setPosition(touchLocation);
//	}

//	if(stepIndex == -5) {
//		CCPoint heroLocation = plane->getPosition();
//		CCPoint lastOne = ccp(heroLocation.x+xdelta, heroLocation.y+ydelta);
//		CCSize winSize = CCDirector::sharedDirector()->getWinSize();
////		if(touchLocation.x>=0 && touchLocation.x<=winSize.width && touchLocation.y>=0 && touchLocation.y<=winSize.height) {
////			if(lastOne.x != touchLocation.x || lastOne.y != touchLocation.y) {
//				plane->setPosition(ccp(heroLocation.x+xdelta, heroLocation.y+ydelta));
////			}
////		}
//	}

}

//触屏结束的时候发射子弹
void GameScene::ccTouchEnded(cocos2d::CCTouch * touch, cocos2d::CCEvent * event) {

	stepIndex = -1;

//	CCPoint location = touch->getLocationInView();
//	location = CCDirector::sharedDirector()->convertToGL(location);
//
//	// Set up initial location of projectile
//	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
//	CCSprite* pBullet = CCSprite::create("bullet.png",
//			CCRectMake(0, 0, 20 ,20));
//	pBullet->setPosition(ccp(20, winSize.height/2));
//
//	// Determinie offset of location to projectile
//	int offX = location.x - pBullet->getPosition().x;
//	int offY = location.y - pBullet->getPosition().y;
//
//	if (offX <= 0)
//		return;
//
//	this->addChild(pBullet);
//
//	// Determine where we wish to shoot the pBullet to
//	int realX = winSize.width + (pBullet->getContentSize().width / 2);
//	float ratio = (float) offY / (float) offX;
//	int realY = (realX * ratio) + pBullet->getPosition().y;
//	CCPoint realDest = ccp(realX, realY);
//
//	// Determine the length of how far we're shooting
//	int offRealX = realX - pBullet->getPosition().x;
//	int offRealY = realY - pBullet->getPosition().y;
//	float length = sqrtf((offRealX * offRealX) + (offRealY * offRealY));
//	float velocity = 480 / 1; // 480pixels/1sec
//	float realMoveDuration = length / velocity;
//
//	// Move pBullet to actual endpoint
//	pBullet->runAction(
//			CCSequence::create(CCMoveTo::create(realMoveDuration, realDest),
//					CCCallFuncN::create(this,
//							callfuncN_selector(GameScene::spriteMoveFinished)),
//					NULL));

}



void GameScene::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {

}



//move finished,删除该节点
void GameScene::spriteMoveFinished(CCNode* sender) {
	CCSprite* sprite = (CCSprite *) sender;
	this->removeChild(sprite, true);
}

//Game logic,动态添加节点
void GameScene::gameLogic(float dt) {
//	this->addTarget();
	this->shootBullet();
}

//碰撞检测
void GameScene::update(CCTime dt) {
	//散播子弹
//	spawmBullets();
//	shootBullet();
}

//为Sprite加上刚体
void GameScene::addBoxBodyForSprite(cocos2d::CCSprite* sprite) {
	//PTM_RATIO  ，这个数一般定义为： 32.0，在box 世界中 是以 米 为单位的，这里是将坐标兑换为box世界中的米，即除以 PTM_RATIO
	// Create physic body for cat
	b2PolygonShape polygon;
	polygon.SetAsBox((float) sprite->getContentSize().width / PT_RATIO / 2,
			(float) sprite->getContentSize().height / PT_RATIO / 2);

	b2FixtureDef spriteShapeDef;
	spriteShapeDef.shape = &polygon;
	spriteShapeDef.density = 10.f;
	spriteShapeDef.isSensor = true;   // 对象之间有碰撞检测但是又不想让它们有碰撞反应
//	spriteShapeDef.isSensor = false;

	b2BodyDef bd;
	bd.type = b2_dynamicBody;
	bd.position = b2Vec2((float) (sprite->getPosition().x / PT_RATIO),
			(float) (sprite->getPosition().y / PT_RATIO));
	bd.userData = sprite;

	b2Body* spriteBody = world->CreateBody(&bd);
	spriteBody->CreateFixture(&spriteShapeDef);

}

//散播子弹
void GameScene::spawmBullets() {
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	CCSprite* cat = CCSprite::create("bullet.png");

	float minY = cat->getContentSize().height / 2;
	float maxY = winSize.height - cat->getContentSize().height / 2;
	float y = minY + rand() % (int) (maxY - minY);

	float startX = winSize.width + cat->getContentSize().width / 2;
	float endX = -cat->getContentSize().width / 2;

	CCPoint startPos = ccp(startX, y);
	CCPoint endPos = ccp(endX, y);

	cat->setPosition(startPos);

	cat->runAction(
			CCSequence::create(CCMoveTo::create(10.f, endPos),
					CCCallFuncN::create(this, callfuncN_selector(GameScene::spriteDone)), NULL));
	//TODO 添加BoxBody
	addBoxBodyForSprite(cat);
	addChild(cat, 1, 111);
}

//随机添加Sprite, 从上到下
void GameScene::addTarget() {
	//TODO 随机选择敌机
	int n = 1;
	CCSprite *target = CCSprite::create("fire1.png");

	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	int minX = target->getContentSize().width / 2;
	int maxX = winSize.width - target->getContentSize().width / 2;

	int rangeX = maxX - minX;

	int actualX = (rand() % rangeX) + minX;

	target->setPosition(ccp(actualX, winSize.height + (target->getContentSize().height/2)));

	//determine speed
	int minDuration = (int) 2.0;
	int maxDuratoin = (int) 4.0;
	int rangeDuration = maxDuratoin - minDuration;
	int actualDuration = (rand() % rangeDuration) + minDuration;

	//create the actions
	CCFiniteTimeAction* actionMove = CCMoveTo::create((float) actualDuration,
			ccp(actualX, 0-target->getContentSize().height/2));
	CCFiniteTimeAction* actionMoveDone = CCCallFuncN::create(this,
			callfuncN_selector(GameScene::spriteDone));
	target->runAction(CCSequence::create(actionMove, actionMoveDone, NULL));
	//TODO 添加Boxbody
	addBoxBodyForSprite(target);
	this->addChild(target, 1, 2);

}

//为Hero发射子弹
void GameScene::shootBullet() {

	float moveDuration = 4.0;

	CCPoint heroPos = this->plane->getPosition();
	CCSprite *cat = CCSprite::create("bullet.png");
	float speed = (cat->getContentSize().height/2);
	CCSize catSize = cat->getContentSize();
	CCSize heroSize = this->plane->getContentSize();

	float startX = heroPos.x;
	float startY = heroPos.y + heroSize.height/2 + catSize.height/2;

	CCPoint startPos = ccp(startX, startY);
//	CCPoint endPos = ccp(startX, -5.0);
	float deltaY = 0 + catSize.height/2;
	CCLog("delata y ;;;;;;%f",deltaY);

//	CCLog("start Pos: x:%f, y:%f \n", startPos.x, startPos.y);
//	CCLog("end Pos: x:%f, y:%f \n", endPos.x, endPos.y);

	cat->setPosition(startPos);
	//TODO 10.f 需要修改
	CCFiniteTimeAction* actionMove = CCMoveTo::create(moveDuration, ccp(startX, deltaY));
	CCFiniteTimeAction* actionMoveDone = CCCallFuncN::create(this,callfuncN_selector(GameScene::spriteMoveFinished));
	cat->runAction(CCSequence::create(actionMove, actionMoveDone, NULL));

//	addBoxBodyForSprite(cat);
	addChild(cat, 1, TAG_Bullet);

}

void GameScene::updateBullet() {
//	vector<CCNode*> nodes = vector(this->getChildByTag(TAG_Bullet));
//	for(vector<CCSprite*>::iterator iter=; iter!=ivec.end(); ++iter)

}


//销毁Sprite
//TODO 调用
void GameScene::spriteDone(CCNode* sender) {

//	CCLog("Sprite Done");
	// sprites被销毁的时候，我们需要销毁Box2d的body
	CCSprite* sprite = dynamic_cast<CCSprite*>(sender);
	if (sprite) {
		b2Body* spriteBody = NULL;
		for (b2Body* b = world->GetBodyList(); b; b = b->GetNext()) {
			if (b->GetUserData() != NULL) {
				CCSprite* curSprite = (CCSprite*) b->GetUserData();
				if (curSprite == sprite) {

//					CCLog("亲，碰撞了！！");
					spriteBody = b;
					removeChild(sprite, true);
					world->DestroyBody(spriteBody);
					break;
				}
			}

			//if (spriteBody)
			//{
			//  world->DestroyBody(spriteBody);
			//}

			// removeChild(sprite, true);
		}
	}
}

//添加粒子发射器，粒子效果
void GameScene::addParticle() {
	CCParticleSystem* m_emitter;
	m_emitter = new CCParticleSystemQuad();
	m_emitter->initWithTotalParticles(50);
	this->addChild(m_emitter, 10);
	m_emitter->setTexture(
			CCTextureCache::sharedTextureCache()->addImage("fire1.png"));
	m_emitter->setDuration(-1);

	// gravity
	m_emitter->setGravity(CCPointZero);

	// angle
	m_emitter->setAngle(90);
	m_emitter->setAngleVar(360);

	// speed of particles
	m_emitter->setSpeed(160);
	m_emitter->setSpeedVar(20);

	// radial
	m_emitter->setRadialAccel(-120);
	m_emitter->setRadialAccelVar(0);

	// tagential
	m_emitter->setTangentialAccel(30);
	m_emitter->setTangentialAccelVar(0);

	// emitter position
	m_emitter->setPosition(CCPointMake(160,240));
	m_emitter->setPosVar(CCPointZero);

	// life of particles
	m_emitter->setLife(4);
	m_emitter->setLifeVar(1);

	// spin of particles
	m_emitter->setStartSpin(0);
	m_emitter->setStartSizeVar(0);
	m_emitter->setEndSpin(0);
	m_emitter->setEndSpinVar(0);

	// color of particles
	ccColor4F startColor = { 0.5f, 0.5f, 0.5f, 1.0f };
	m_emitter->setStartColor(startColor);

	ccColor4F startColorVar = { 0.5f, 0.5f, 0.5f, 1.0f };
	m_emitter->setStartColorVar(startColorVar);

	ccColor4F endColor = { 0.1f, 0.1f, 0.1f, 0.2f };
	m_emitter->setEndColor(endColor);

	ccColor4F endColorVar = { 0.1f, 0.1f, 0.1f, 0.2f };
	m_emitter->setEndColorVar(endColorVar);

	// size, in pixels
	m_emitter->setStartSize(80.0f);
	m_emitter->setStartSizeVar(40.0f);
	m_emitter->setEndSize(kParticleStartSizeEqualToEndSize);

	// emits per second
	m_emitter->setEmissionRate(
			m_emitter->getTotalParticles() / m_emitter->getLife());

	// additive
	m_emitter->setBlendAdditive(true);

}

//没周期检测碰撞 判断 销毁实体
void GameScene::tick(float dt) {
	if (world) {
		world->Step(dt, 10, 10);
	}

// 基于cocos2d的精灵位置来更新box2d的body位置
	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext()) {
		if (b->GetUserData() != NULL) {
			CCSprite* sprite = (CCSprite*) b->GetUserData();
			if (sprite) {
				b2Vec2 pt = b2Vec2((float) (sprite->getPosition().x / PT_RATIO),
						(float) (sprite->getPosition().y / PT_RATIO));
				float angle =
						(float) CC_DEGREES_TO_RADIANS(sprite->getRotation());
				b->SetTransform(pt, angle);
			}
		}
	}

	std::list<b2Body*> toDestroy_list;

	for (std::list<MyContact>::iterator it = contactListener->contact_list.begin(); it != contactListener->contact_list.end(); ++it) {
		MyContact& contact = *it;

		b2Body* bodyA = contact.fixtureA->GetBody();
		b2Body* bodyB = contact.fixtureB->GetBody();

		CCSprite* sa = (CCSprite*) bodyA->GetUserData();
		CCSprite* sb = (CCSprite*) bodyB->GetUserData();

		if (sa && sb) {
			if (sa->getTag() == 1 && sb->getTag() == 2) {
				//TODO 只执行这个,有问题
				CCLog("To Destroy sb");
				toDestroy_list.push_back(bodyB);
			} else if (sa->getTag() == 2 && sa->getTag() == 1) {
				CCLog("To Destroy sa");
				toDestroy_list.push_back(bodyA);
			}
		}
	}

	//销毁碰撞的实体
	std::list<b2Body*>::iterator it = toDestroy_list.begin();
	while (it != toDestroy_list.end()) {
		if ((*it)->GetUserData() != NULL) {
			CCSprite* sprite = (CCSprite*) ((*it)->GetUserData());
			if (sprite) {
				removeChild(sprite, true);
			}
			world->DestroyBody(*it);
		}
		++it;
	}

	toDestroy_list.clear();

}

//每周期更新滑动背景
void GameScene::updateBackGround() {
	int scroll_speed = 3;
	CCSize size = CCDirector::sharedDirector()->getWinSize();
//	CCLog("win size(width) : %d",size.width);
//	CCLog("win size(height) : %d",size.height);

	CCPoint backPoint1 = backSprite1->getPosition();
	CCPoint backPoint2 = backSprite2->getPosition();
	backSprite1->setPosition(ccp(backPoint1.x, backPoint1.y-scroll_speed));
	backSprite2->setPosition(ccp(backPoint2.x, backPoint2.y-scroll_speed));
////
	if(backPoint2.y < 0) {
		float temp = backPoint2.y + size.height;
		backSprite1->setPosition(ccp(backPoint1.x, temp));
	}
	if(backPoint1.y < 0) {
		float temp = backPoint1.y + size.height;
		backSprite2->setPosition(ccp(backPoint2.x, temp));
	}

}




