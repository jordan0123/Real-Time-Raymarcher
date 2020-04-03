#ifndef SCENE_H
#define SCENE_H

#include "myvec.h"
#include <SFML/Graphics.hpp>

class Scene
{
public:
	sf::Shader* shader;

	myvec::Vec3d cam_pos;
	myvec::Vec3d cam_forward;
	myvec::Vec3d cam_up;
	myvec::Vec3d cam_right;
	
	float cam_focal_length;
	//int max_ray_steps;
	//float min_ray_dist;

	float scale;

public:
	Scene(sf::Shader* s);
	~Scene();

	void SetCamAng(myvec::Vec3d ang); // absolute
	void TurnCam(myvec::Vec3d ang); // additive P, Y, R

	void SetCamPos(myvec::Vec3d pos); // absolute
	void MoveCam(myvec::Vec3d v); // additive

	void SetCamFocalLength(float f) { cam_focal_length = f; }

	void UpdateCam();

	myvec::Vec3d CamGetForward() { return cam_forward; }
	myvec::Vec3d CamGetRight() { return cam_right; }
	myvec::Vec3d CamGetUp() { return cam_up; }

	myvec::Vec3d CamGetPos() { return cam_pos; }
};

#endif
