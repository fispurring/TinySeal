#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include "cocos2d.h"
#include "Box2D.h"
#include "GLES-Render.h"

using namespace cocos2d;

#define kMaxHillKeyPoints 1000
#define kHillSegmentWidth 5

#define kMaxHillVertices 4000
#define kMaxBorderVertices 800

class Terrain:public CCNode
{
	int _offsetX;
	CCPoint _hillKeyPoints[kMaxHillKeyPoints];
	int _fromKeyPointI;
	int _toKeyPointI;

	int _nHillVertices;
	ccVertex2F _hillVertices[kMaxHillVertices];
	ccVertex2F _hillTexCoords[kMaxHillVertices];

	int _nBorderVertices;
	CCPoint _borderVertices[kMaxBorderVertices];

	b2World *_world;
	b2Body *_body;
	GLESDebugDraw *_debugDraw;

	void generateHills();
	void resetHillVertices();
	void resetBox2DBody();
	void setupDebugDraw();
public:
	~Terrain();
	CC_SYNTHESIZE_RETAIN(CCSprite *,_stripes,Stripes);

	CREATE_FUNC(Terrain);
	void setOffsetX(float newOffsetX);
	virtual bool init();
	virtual void draw(); 
	bool initWithWorld(b2World *world);
};

#endif