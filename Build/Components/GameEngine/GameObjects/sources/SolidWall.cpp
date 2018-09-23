# include "../includes/SolidWall.hpp"

SolidWall::SolidWall( eGameObjectType type, Vector3d *position ){
	this->strType = "SolidWall";
	this->position = position;
	this->eType = type;
	this->state = ALIVE;
}

SolidWall::~SolidWall( void ){

}