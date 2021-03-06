#include "Vector3d.hpp"

Vector3d::Vector3d( float vX, float vY, float vZ): vX(vX), vY(vY), vZ(vZ){
}

Vector3d::Vector3d( void ){
}

Vector3d::Vector3d(Vector3d const & src){
	this->vX = src.vX;
	this->vY = src.vY;
	this->vZ = src.vZ;
}

Vector3d	&Vector3d::operator=(Vector3d const &rhs) {
	return *(new Vector3d(rhs));
}

Vector3d::~Vector3d( void ){
}

void	Vector3d::incX( float num){
	this->vX += num;
}

void	Vector3d::incY( float num){
	this->vY += num;
}

void	Vector3d::incZ( float num){
	this->vZ += num;
}

void	Vector3d::decX( float num){
	this->vX -= num;
}

void	Vector3d::decY( float num){
	this->vY -= num;
}

void	Vector3d::decZ( float num){
	this->vZ -= num;
}

void	Vector3d::setAll( float vX, float vY, float vZ){
	this->vX = vX;
	this->vY = vY;
	this->vZ = vZ;
}