#include "Terrain.h"
#include "HelloWorldScene.h"

Terrain::~Terrain()
{
	_stripes->release();
	_stripes=NULL;
}

void Terrain::setupDebugDraw()
{
	_debugDraw=new GLESDebugDraw(PTM_RATIO*CCDirector::sharedDirector()->getContentScaleFactor());
	_world->SetDebugDraw(_debugDraw);
	_debugDraw->SetFlags(b2Draw::e_shapeBit|b2Draw::e_jointBit);
}

bool Terrain::initWithWorld(b2World *world)
{
	_fromKeyPointI=0;
	_toKeyPointI=0;
	_body=NULL;
	_stripes=CCSprite::create();
	m_pShaderProgram=CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTexture);
	m_pShaderProgram->retain();
	_world=world;
	this->setupDebugDraw();
	this->generateHills();
	this->resetHillVertices();

	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("TinySeal.plist");
	return true;
}

bool Terrain::init()
{
	_fromKeyPointI=0;
	_toKeyPointI=0;
	_stripes=CCSprite::create();
	m_pShaderProgram=CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTexture);
	m_pShaderProgram->retain();
	generateHills();
	resetHillVertices();

	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("TinySeal.plist");
	return true;
}

void Terrain::draw()
{
	CCNode::draw();
	CC_NODE_DRAW_SETUP();
	
	ccGLBlendFunc( CC_BLEND_SRC, CC_BLEND_DST );
	ccGLEnableVertexAttribs(kCCVertexAttribFlag_TexCoords|kCCVertexAttribFlag_Position);
	ccGLBindTexture2D(_stripes->getTexture()->getName());
	glVertexAttribPointer(kCCVertexAttrib_Position,2,GL_FLOAT,GL_FALSE,0,_hillVertices);;
	glVertexAttribPointer(kCCVertexAttrib_TexCoords,2,GL_FLOAT,GL_FALSE,0,_hillTexCoords);
	glDrawArrays(GL_TRIANGLE_STRIP,0,(GLsizei)_nHillVertices);

	/*for(int i=1;i<kMaxHillKeyPoints;++i) {
		ccDrawLine(_hillKeyPoints[i-1],_hillKeyPoints[i]);
	}*/
//	for (int i =max(_fromKeyPointI,1); i <=_toKeyPointI; ++i)
//	{
//		ccDrawColor4F(1.0,0,0,1.0);
//		ccDrawLine(_hillKeyPoints[i],_hillKeyPoints[i-1]);
//
//		ccDrawColor4F(1.0,1.0,1.0,1.0);
//
//		CCPoint p0=_hillKeyPoints[i-1];
//		CCPoint p1=_hillKeyPoints[i];
//		int hSegments=floorf((p1.x-p0.x)/kHillSegmentWidth);
//		float dx=(p1.x-p0.x)/hSegments;
//		float da=M_PI/hSegments;
//		float ymid=(p0.y+p1.y)/2;
//		float ampl=(p0.y-p1.y)/2;
//
//		CCPoint pt0,pt1;
//		pt0=p0;
//		for (int j = 0; j < hSegments+1; ++j)
//		{
//			pt1.x=p0.x+j*dx;
//			pt1.y=ymid+ampl*cosf(da*j);
//
//			ccDrawLine(pt0,pt1);
//			pt0=pt1;
//		}
//	}

	//_world->DrawDebugData();
}

void Terrain::resetBox2DBody()
{
	if (_body)
	{
		_world->DestroyBody(_body);
	}

	b2BodyDef bd;
	bd.position.Set(0,0);

	_body=_world->CreateBody(&bd);

	b2EdgeShape shape;

	b2Vec2 p1,p2;
	for (int i = 0; i < _nBorderVertices-1; i++)
	{
		p1=b2Vec2(_borderVertices[i].x/PTM_RATIO,_borderVertices[i].y/PTM_RATIO);
		p2=b2Vec2(_borderVertices[i+1].x/PTM_RATIO,_borderVertices[i+1].y/PTM_RATIO);
		shape.Set(p1,p2);
		_body->CreateFixture(&shape,0);
	}

	/*if (_body)
	{
		return;
	}

	CCPoint p0=_hillKeyPoints[0];
	CCPoint p1=_hillKeyPoints[kMaxHillKeyPoints-1];

	b2BodyDef bd;
	bd.position.Set(0,0);
	_body=_world->CreateBody(&bd);

	b2EdgeShape shape;
	b2Vec2 ep1=b2Vec2(p0.x/PTM_RATIO,0);
	b2Vec2 ep2=b2Vec2(p1.x/PTM_RATIO,0);
	shape.Set(ep1,ep2);
	_body->CreateFixture(&shape,0);*/
}

void Terrain::generateHills()
{
	srand(time(0));
	CCSize winSize=CCDirector::sharedDirector()->getWinSize();

	float minDX=160;
	float minDY=60;
	int rangeDX=80;
	int rangeDY=40;

	float x=-minDX;
	float y=winSize.width/2-minDY;

	float dy,ny;
	float sign=1;
	float paddingTop=20;
	float paddingBottom=20;

	for(int i=0;i<kMaxHillKeyPoints;++i) {
		_hillKeyPoints[i]=ccp(x,y);
		if(i==0) {
			x=0;
			y=winSize.height/2;
		}
		else {
			x+=rand()%rangeDX+minDX;
			while(true) {
				dy=rand()%rangeDY+minDY;
				ny=y+dy*sign;
				if(ny<winSize.height-paddingTop&&ny>paddingBottom) {
					break;
				}
			}
			y=ny;
		}
		sign*=-1;
		/*x+=winSize.width/2;
		y=rand()%(int)winSize.height;*/
	}
}

void Terrain::setOffsetX(float newOffsetX)
{
	CCSize winSize=CCDirector::sharedDirector()->getWinSize();

	_offsetX=newOffsetX;
	this->setPosition(ccp(winSize.width/8-_offsetX*this->getScale(),0));
	resetHillVertices();
}

void Terrain::resetHillVertices()
{
	CCSize winSize=CCDirector::sharedDirector()->getWinSize();
	static int prevFromKeyPointI=-1;
	static int prevToKeyPointI=-1;

	while(_hillKeyPoints[_fromKeyPointI+1].x<_offsetX-winSize.width/8/this->getScale()) {
		_fromKeyPointI++;
	}

	while (_hillKeyPoints[_toKeyPointI].x<_offsetX+winSize.width*9/8/this->getScale())
	{
		_toKeyPointI++;
	}

	if (prevFromKeyPointI!=_fromKeyPointI||prevToKeyPointI!=_toKeyPointI)
	{
		_nHillVertices=0;
		_nBorderVertices=0;
		CCPoint p0,p1,pt0,pt1;
		p0=_hillKeyPoints[_fromKeyPointI];
		for (int i = _fromKeyPointI+1; i < _toKeyPointI+1; ++i)
		{
			p1=_hillKeyPoints[i];

			int hSegments=floorf((p1.x-p0.x)/kHillSegmentWidth);
			float dx=(p1.x-p0.x)/hSegments;
			float da=M_PI/hSegments;
			float ymid=(p0.y+p1.y)/2;
			float ampl=(p0.y-p1.y)/2;
			pt0=p0;
			_borderVertices[_nBorderVertices++]=pt0;
			for (int j = 1; j < hSegments+1; ++j)
			{
				pt1.x=p0.x+j*dx;
				pt1.y=ymid+ampl*cosf(da*j);
				_borderVertices[_nBorderVertices++]=pt1;

				_hillVertices[_nHillVertices] = vertex2(pt0.x, 0);
                _hillTexCoords[_nHillVertices++] = vertex2(pt0.x/512, 1.0f);
                _hillVertices[_nHillVertices] = vertex2(pt0.x, pt0.y);
                _hillTexCoords[_nHillVertices++] = vertex2(pt0.x/512, 0.0f);   
				pt0=pt1;
			}
			p0=p1;
		}
		prevFromKeyPointI=_fromKeyPointI;
		prevToKeyPointI=_toKeyPointI;

		this->resetBox2DBody();
	}
}