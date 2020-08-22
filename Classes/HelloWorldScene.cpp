/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

#define BACKGROUND_MUSIC_SFX  "background-music-aac.mp3"
#define PEW_PEW_SFX           "pew-pew-lei.mp3"

using namespace CocosDenshion;

USING_NS_CC;

enum class PhysicsCategory {
    None = 0,
    Monster = (1 << 0),    // 1
    Projectile = (1 << 1), // 2
    All = PhysicsCategory::Monster | PhysicsCategory::Projectile // 3
};


Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    if (!Scene::initWithPhysics())
    {
        return false;
    }
    this->getPhysicsWorld()->setGravity(Vec2(0,0));
    this->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    auto visibleSize = Director::getInstance()->getVisibleSize();

    auto background = DrawNode::create();
    background->drawSolidRect(origin, visibleSize, Color4F(0.6, 0.6, 0.6, 1.0));
    this->addChild(background);

    m_player = Sprite::create("player.png");
    m_player->setPosition(Vec2(visibleSize.width * 0.1, visibleSize.height * 0.5));
    this->addChild(m_player);

    srand(time(NULL));
    this->schedule(schedule_selector(HelloWorld::addMonster), 1.5);


    auto eventListener = EventListenerTouchOneByOne::create();
    eventListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, m_player);

    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::onContactBegan, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);

    SimpleAudioEngine::getInstance()->playBackgroundMusic(BACKGROUND_MUSIC_SFX, true);

    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}

void HelloWorld::addMonster(float dt) {
    auto monster = Sprite::create("monster.png");
    auto monsterSize = monster->getContentSize();
    auto physicsBody = PhysicsBody::createBox(Size(monsterSize.width , monsterSize.height), PhysicsMaterial(0.1f, 1.0f, 0.0f));
// 2
    physicsBody->setDynamic(true);
// 3
    physicsBody->setCategoryBitmask((int)PhysicsCategory::Monster);
    physicsBody->setCollisionBitmask((int)PhysicsCategory::None);
    physicsBody->setContactTestBitmask((int)PhysicsCategory::Projectile);

    monster->setPhysicsBody(physicsBody);



    auto selfContentSize = this->getContentSize();
    int minY = monsterSize.height/2;
    int maxY = selfContentSize.height - monsterSize.height/2;
    int rangeY = maxY - minY;
    int randomY = (rand() % rangeY) + minY;
    monster->setPosition(Vec2(selfContentSize.width + monsterSize.width/2, randomY));
    this->addChild(monster);

    // 2
    int minDuration = 2.0;
    int maxDuration = 4.0;
    int rangeDuration = maxDuration - minDuration;
    int randomDuration = (rand() % rangeDuration) + minDuration;

    // 3
    auto actionMove = MoveTo::create(randomDuration, Vec2(-monsterSize.width/2, randomY));
    auto actionRemove = RemoveSelf::create();
    monster->runAction(Sequence::create(actionMove,actionRemove, nullptr));


}

bool HelloWorld::onTouchBegan(Touch *touch, Event *unused_event) {
    // 1  - Just an example for how to get the  _player object
    auto player = unused_event->getCurrentTarget();

    // 2
    Vec2 touchLocation = touch->getLocation();
    Vec2 offset = touchLocation - player->getPosition();

    // 3
    if (offset.x < 0) {
        return true;
    }

    // 4
    auto projectile = Sprite::create("projectile.png");
    projectile->setPosition(player->getPosition());
    auto projectileSize = projectile->getContentSize();
    auto physicsBody = PhysicsBody::createCircle(projectileSize.width / 2);
    physicsBody->setDynamic(true);
    physicsBody->setCategoryBitmask((int)PhysicsCategory::Projectile);
    physicsBody->setCollisionBitmask((int)PhysicsCategory::None);
    physicsBody->setContactTestBitmask((int)PhysicsCategory::Monster);
    projectile->setPhysicsBody(physicsBody);
    this->addChild(projectile);

    // 5
    offset.normalize();
    auto shootAmount = offset * 1000;

    // 6
    auto realDest = shootAmount + projectile->getPosition();

    // 7
    auto actionMove = MoveTo::create(2.0f, realDest);
    auto actionRemove = RemoveSelf::create();
    projectile->runAction(Sequence::create(actionMove, actionRemove, nullptr));

    SimpleAudioEngine::getInstance()->playEffect(PEW_PEW_SFX);

    return true;
}

bool HelloWorld::onContactBegan(PhysicsContact &contact) {
    auto nodeA = contact.getShapeA()->getBody()->getNode();
    auto nodeB = contact.getShapeB()->getBody()->getNode();

    nodeA->removeFromParent();
    nodeB->removeFromParent();
    return true;
}
