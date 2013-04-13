#include "Hero.h"

void Hero::createBody()
{
	float radius=16.0f;
	CCSize size=CCDirector::sharedDirector()->getWinSize();
	int screenH=size.height;

	CCPoint startPositioin=ccp(0,screenH/2+radius);

	b2BodyDef bd;
	bd.type=b2_dynamicBody;
	bd.linearDamping=0.1f;
	bd.fixedRotation=true;
	bd.position.Set(startPositioin.x/PTM_RATIO,startPositioin.y/PTM_RATIO);
	_body=_world->CreateBody(&bd);

	b2CircleShape shape;
	shape.m_radius=radius/PTM_RATIO;

	b2FixtureDef fd;
	fd.shape=&shape;
	fd.density=1.0f;
	fd.restitution=0.0f;
	fd.friction=0.2;
	_body->CreateFixture(&fd);
}

bool Hero::initWithWorld(b2World *world)
{
	if (!CCSprite::initWithSpriteFrameName("seal1.png"))
	{
		return false;
	}
	_awake=false;
	_world=world;
	createBody();
	_nextVel=0;
	_normalAnimate=NULL;

	_normalAnim=CCAnimation::create();
	_normalAnim->addSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("seal1.png"));
	_normalAnim->addSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("seal2.png"));

	_normalAnim->setDelayPerUnit(0.1);
	_normalAnim->retain();
	return true;
}

void Hero::update()
{
	this->setPosition(ccp(_body->GetPosition().x*PTM_RATIO,_body->GetPosition().y*PTM_RATIO));
	b2Vec2 vel=_body->GetLinearVelocity();
	b2Vec2 weightVel=vel;

	for (int i = 0; i < NUM_PREV_VELS; ++i)
	{
		weightVel+=_prevVels[i];
	}
	weightVel=b2Vec2(weightVel.x/NUM_PREV_VELS,weightVel.y/NUM_PREV_VELS);
	_prevVels[_nextVel++]=vel;
	if (_nextVel>=NUM_PREV_VELS)
	{
		_nextVel=0;
	}

	float angle=ccpToAngle(ccp(weightVel.x,weightVel.y));
	if (_awake)
	{
		setRotation(-1*CC_RADIANS_TO_DEGREES(angle));
	}
}

void Hero::wake()
{
	_awake=true;
	_body->SetActive(true);
	_body->ApplyLinearImpulse(b2Vec2(1,2),_body->GetPosition());
}

void Hero::dive()
{
	_body->ApplyForce(b2Vec2(5,-50),_body->GetPosition());
}

void Hero::limitVelocity()
{
	if (!_awake)
	{
		return;
	}

	const float minVelocityX=5;
	const float minVelocityY=-40;
	b2Vec2 vel=_body->GetLinearVelocity();

	if (vel.x<minVelocityX)
	{
		vel.x=minVelocityX;
	}

	_body->SetLinearVelocity(vel);
}

void Hero::runNormalAnimation()
{
	if (_normalAnimate||!_awake)
	{
		return;
	}
	_normalAnimate=CCRepeatForever::create(CCAnimate::create(_normalAnim));
	this->runAction(_normalAnimate);
}

void Hero::runForceAnimation()
{
	if (_normalAnimate)
	{
		this->stopAction(_normalAnimate);
		_normalAnimate=NULL;
		this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("seal_downhill.png"));
	}
}

void Hero::nodive()
{
	runNormalAnimation();
}

