#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

#include "Box2D/Box2D.h"

#include "SimpleAudioEngine.h"
#include "Terrain.h"
#include "Hero.h"

using namespace cocos2d;

#define PTM_RATIO 32.0

class HelloWorld : public cocos2d::CCLayer
{
	CCSprite *_background;
	Terrain *_terrain;
	b2World *_world;
	Hero *_hero;
	bool _tapDown;

	void setupShader();
	void setupWorld();
	void createTestBodyAtPosition(CCPoint position);
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);

    // implement the "static node()" method manually
    CREATE_FUNC(HelloWorld);

	CCSprite *CreateSpriteWithColor(ccColor4F bgColor,float size);
	CCSprite *CreateStripedSpriteWithColor1(ccColor4F c1,ccColor4F c2,float size,int nStripes);

	ccColor4F randomBrightColor();

	void genBackground();

	virtual void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesEnded(CCSet *pTouches,CCEvent *pEvent);
	virtual void ccTouchesCancelled(CCSet *pTouches,CCEvent *pEvent);
	virtual void update(float delta);
};

#endif  // __HELLOWORLD_SCENE_H__