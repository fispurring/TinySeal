#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;

CCScene* HelloWorld::scene()
{
    CCScene * scene = NULL;
    do 
    {
        // 'scene' is an autorelease object
        scene = CCScene::create();
        CC_BREAK_IF(! scene);

        // 'layer' is an autorelease object
        HelloWorld *layer = HelloWorld::create();
        CC_BREAK_IF(! layer);

        // add layer as a child to scene
        scene->addChild(layer);
    } while (0);

    // return the scene
    return scene;
}

void HelloWorld::setupWorld()
{
	b2Vec2 gravity=b2Vec2(0.0f,-7.0f);
	_world=new b2World(gravity);
}

void HelloWorld::createTestBodyAtPosition(CCPoint position)
{
	b2BodyDef testBodyDef;
	testBodyDef.type=b2_dynamicBody;
	testBodyDef.position.Set(position.x/PTM_RATIO,position.y/PTM_RATIO);
	b2Body *testBody=_world->CreateBody(&testBodyDef);

	b2CircleShape testBodyShape;
	b2FixtureDef testFixtureDef;
	testBodyShape.m_radius=25.0/PTM_RATIO;
	testFixtureDef.shape=&testBodyShape;
	testFixtureDef.density=1.0;
	testFixtureDef.restitution=0.5;
	testBody->CreateFixture(&testFixtureDef);
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    if(! CCLayer::init()) {
		return false;
	}

	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("TinySeal.plist");

	_tapDown=false;
	this->setupWorld();
	_background=NULL;

	//_terrain=Terrain::create();
	_terrain=new Terrain();
	_terrain->initWithWorld(_world);
	_terrain->autorelease();

	this->addChild(_terrain,1);
	
	this->genBackground();
	this->setTouchEnabled(true);

	this->scheduleUpdate();

	_hero=new Hero();
	_hero->initWithWorld(_world);
	_hero->autorelease();
	_terrain->addChild(_hero);

	//this->setScale(0.25);
	CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("Sounds/TinySeal.caf",true);
    return true;
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    // "close" menu item clicked
    CCDirector::sharedDirector()->end();
}

CCSprite *HelloWorld::CreateSpriteWithColor(ccColor4F bgColor,float size)
{
	CCRenderTexture* rt=CCRenderTexture::create(size,size);
	rt->beginWithClear(bgColor.r,bgColor.g,bgColor.b,bgColor.a);

	float gradientAlpha = 0.7;    
	ccVertex2F vertices[4];
	ccColor4F colors[4];
	int nVertices = 0;
 
	vertices[nVertices] = vertex2(0, 0);
	colors[nVertices++] = ccc4f(0, 0, 0, 0 );
	vertices[nVertices] = vertex2(size, 0);
	colors[nVertices++] = ccc4f(0, 0, 0, 0);
	vertices[nVertices] = vertex2(0, size);
	colors[nVertices++] = ccc4f(0, 0, 0, gradientAlpha);
	vertices[nVertices] = vertex2(size, size);
	colors[nVertices++] = ccc4f(0, 0, 0, gradientAlpha);
 
	CC_NODE_DRAW_SETUP();
	ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position | kCCVertexAttribFlag_Color );
	glVertexAttribPointer(kCCVertexAttrib_Position,2, GL_FLOAT,GL_FALSE, 0, vertices);
	glVertexAttribPointer(kCCVertexAttrib_Color,4, GL_FLOAT,GL_FALSE, 0, colors);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)nVertices);

	CCSprite *noise=CCSprite::create("Noise.png");
	ccBlendFunc blendFunc={GL_DST_COLOR,GL_ZERO};
	noise->setBlendFunc(blendFunc);
	noise->setPosition(ccp(size/2,size/2));
	noise->visit();

	rt->end();
	return CCSprite::createWithTexture(rt->getSprite()->getTexture());
}

CCSprite *HelloWorld::CreateStripedSpriteWithColor1(ccColor4F c1,ccColor4F c2,float size,int nStripes)
{
	CCRenderTexture *rt=CCRenderTexture::create(size,size);
	rt->beginWithClear(c1.r,c1.g,c1.b,c1.a);

	ccVertex2F *vertices=new ccVertex2F[nStripes*6];
	ccColor4F *colors=new ccColor4F[nStripes*6];
	int nVertices=0;
	float x1=-size;
	float x2;
	float y1=size;
	float y2=0;
	float dx=size/nStripes*2;
	float stripeWidth=dx/2;
	for(int i=0;i<nStripes;++i)
	{
		x2=x1+size;
		vertices[nVertices]=vertex2(x1,y1);
		colors[nVertices++]=ccc4f(c2.r,c2.g,c2.b,c2.a);
		vertices[nVertices]=vertex2(x1+stripeWidth,y1);
		colors[nVertices++]=ccc4f(c2.r,c2.g,c2.b,c2.a);
		vertices[nVertices]=vertex2(x2,y2);
		colors[nVertices++]=ccc4f(c2.r,c2.g,c2.b,c2.a);
		vertices[nVertices]=vertices[nVertices-2];
		colors[nVertices++]=ccc4f(c2.r,c2.g,c2.b,c2.a);
		vertices[nVertices]=vertices[nVertices-2];
		colors[nVertices++]=ccc4f(c2.r,c2.g,c2.b,c2.a);
		vertices[nVertices]=vertex2(x2+stripeWidth,y2);
		colors[nVertices++]=ccc4f(c2.r,c2.g,c2.b,c2.a);
		x1+=dx;
	}
	CC_NODE_DRAW_SETUP();
	ccGLEnableVertexAttribs(kCCVertexAttribFlag_Position|kCCVertexAttribFlag_Color);

	glVertexAttribPointer(kCCVertexAttrib_Position,2,GL_FLOAT,GL_FALSE,0,vertices);
	glVertexAttribPointer(kCCVertexAttrib_Color,4,GL_FLOAT,GL_FALSE,0,colors);
	glDrawArrays(GL_TRIANGLES,0,(GLsizei)nVertices);

	float gradientAlpha=0.7;
	nVertices=0;

	vertices[nVertices]=vertex2(0,0);
	colors[nVertices++]=ccc4f(0,0,0,0);
	vertices[nVertices]=vertex2(size,0);
	colors[nVertices++]=ccc4f(0,0,0,0);
	vertices[nVertices]=vertex2(0,size);
	colors[nVertices++]=ccc4f(0,0,0,gradientAlpha);
	vertices[nVertices]=vertex2(size,size);
	colors[nVertices++]=ccc4f(0,0,0,gradientAlpha);

	ccGLBlendFunc( CC_BLEND_SRC, CC_BLEND_DST );
	glVertexAttribPointer(kCCVertexAttrib_Position,2,GL_FLOAT,GL_FALSE,0,vertices);
	glVertexAttribPointer(kCCVertexAttrib_Color,4,GL_FLOAT,GL_FALSE,0,colors);
	glDrawArrays(GL_TRIANGLE_STRIP,0,nVertices);

	float borderWidth=size/16;
	float borderAlpha=0.3f;
	nVertices=0;

	vertices[nVertices]=vertex2(0,0);
	colors[nVertices++]=ccc4f(1,1,1,borderAlpha);
	vertices[nVertices]=vertex2(size,0);
	colors[nVertices++]=ccc4f(1,1,1,borderAlpha);

	vertices[nVertices]=vertex2(0,borderWidth);
	colors[nVertices++]=ccc4f(0,0,0,0);
	vertices[nVertices]=vertex2(size,borderWidth);
	colors[nVertices++]=ccc4f(0,0,0,0);

	glVertexAttribPointer(kCCVertexAttrib_Position,2,GL_FLOAT,GL_FALSE,0,vertices);
	glVertexAttribPointer(kCCVertexAttrib_Color,4,GL_FLOAT,GL_FALSE,0,colors);
	glDrawArrays(GL_TRIANGLE_STRIP,0,nVertices);

	CCSprite *noise=CCSprite::create("Noise.png");
	ccBlendFunc blendFunc={GL_DST_COLOR,GL_ZERO};
	noise->setBlendFunc(blendFunc);
	noise->setPosition(ccp(size/2,size/2));
	noise->visit();

	delete[] colors;
	delete[] vertices;
	rt->end();
	return CCSprite::createWithTexture(rt->getSprite()->getTexture());
}

ccColor4F HelloWorld::randomBrightColor()
{
	while(true)
	{
		float requireBrightness=192;
		ccColor4B randomColor=ccc4(rand()%255,rand()%255,rand()%255,255);
		if(randomColor.r>requireBrightness||
			randomColor.g>requireBrightness||
			randomColor.b>requireBrightness) {
			return ccc4FFromccc4B(randomColor);
		}
	}
}

void HelloWorld::genBackground()
{
	if(_background)	{
		_background->removeFromParentAndCleanup(true);
	}

	setupShader();
	srand(clock());

	ccColor4F bgColor=randomBrightColor();
	_background=CreateSpriteWithColor(bgColor,512);
	CCSize winSize=CCDirector::sharedDirector()->getWinSize();
	_background->setPosition(ccp(winSize.width/2,winSize.height/2));
	ccTexParams tp={GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT};
	_background->getTexture()->setTexParameters(&tp);
	this->addChild(_background);

	ccColor4F color3=randomBrightColor();
	ccColor4F color4=randomBrightColor();
	CCSprite *stripes=CreateStripedSpriteWithColor1(color3,color4,512,4);
	ccTexParams tp2={GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_CLAMP_TO_EDGE};
	stripes->getTexture()->setTexParameters(&tp2);
	_terrain->setStripes(stripes);
}

void HelloWorld::update(float delta)
{
	if (_tapDown)
	{
		if (!_hero->getAwake())
		{
			_hero->wake();
			_tapDown=false;
		}
		else {
			_hero->dive();
		}
	}
	else {
		_hero->nodive();
	}
	_hero->limitVelocity();

	static double UPDATE_INTERVAL=1.0f/60.0f;
	static double MAX_CYCLES_PER_FRAME=5;
	static double timeAccumulator=0;

	timeAccumulator+=delta;
	if (timeAccumulator>(MAX_CYCLES_PER_FRAME*UPDATE_INTERVAL))
	{
		timeAccumulator=UPDATE_INTERVAL;
	}

	int32 velocityIterations=3;
	int32 positionIterations=2;
	while (timeAccumulator>=UPDATE_INTERVAL)
	{
		timeAccumulator-=UPDATE_INTERVAL;
		_world->Step(UPDATE_INTERVAL,velocityIterations,positionIterations);
		//_world->ClearForces();
	}

	/*float PIXELS_PER_SECOND=100;
	static float offset=0;
	offset+=PIXELS_PER_SECOND*delta;*/

	_hero->update();
	CCSize winSize=CCDirector::sharedDirector()->getWinSize();
	float scale=(winSize.height*3/4)/_hero->getPositionY();
	if (scale>1)
	{
		scale=1;
	}
	_terrain->setScale(scale);

	float offset=_hero->getPositionX();

	CCSize textureSize=_background->getTexture()->getContentSize();
	_background->setTextureRect(CCRectMake(offset*0.7,0,textureSize.width,textureSize.height));

	_terrain->setOffsetX(offset);
}

void HelloWorld::setupShader()
{
	m_pShaderProgram=CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionColor);
	m_pShaderProgram->retain();
}

void HelloWorld::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
	genBackground();
	_tapDown=true;
	_hero->runForceAnimation();
	/*CCTouch *anyTouch=(CCTouch*)pTouches->anyObject();
	CCPoint touchLocation=_terrain->convertTouchToNodeSpace(anyTouch);
	this->createTestBodyAtPosition(touchLocation);*/
}

void HelloWorld::ccTouchesEnded(CCSet *pTouches,CCEvent *pEvent)
{
	_tapDown=false;
	_hero->runNormalAnimation();
}

void HelloWorld::ccTouchesCancelled(CCSet *pTouches,CCEvent *pEvent)
{
	_tapDown=false;
}