#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include "myvec.h"
#include "scene.h"

bool quit = false;

int window_width = 1920;
int window_height = 1080;

sf::Vector2i mouse_pos;
bool mouse_is_locked = false;
float move_speed = 0.001;
float mouse_sens = 0.0015;

void UpdateScene(sf::RenderWindow* window, sf::Shader* shader, Scene* scene)
{
	window->setActive(true);

	// create screen rectangle
	sf::RectangleShape rect;
	rect.setSize(sf::Vector2f(window_width, window_height));
	rect.setPosition(0, 0);

	while (!quit)
	{
		std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(1);

		scene->UpdateCam();

		window->clear();
		window->draw(rect, shader);
		window->display();

		std::this_thread::sleep_until(t1);
	}
}

int main(int argc, char** argv)
{
	// check if gpu supports shaders
	if (!sf::Shader::isAvailable()) {
		std::cout << "GPU does not support shaders." << std::endl;
		return EXIT_FAILURE;
	}

	sf::Shader shader;

	// Load fragment shader
	if (!shader.loadFromFile("frag.glsl", sf::Shader::Fragment)) {
		std::cout << "Failed to compile fragment shader." << std::endl;
		return EXIT_FAILURE;
	}

	shader.setUniform("resolution", sf::Glsl::Vec2(window_width, window_height));

	// create the window
	sf::RenderWindow window(sf::VideoMode(window_width, window_height), "test");

	sf::Vector2i screen_center(window.getSize().x / 2, window.getSize().y / 2);
	//LockMouse(window);
	mouse_is_locked = true;
	window.setMouseCursorVisible(false);

	// create scene
	Scene scene = Scene(&shader);

	window.setActive(false); // yield window to render thread
	std::thread scene_dispatcher(UpdateScene, &window, &shader, &scene);

	myvec::Vec3d acc = myvec::Vec3d();
	myvec::Vec3d vel = myvec::Vec3d();
	myvec::Vec3d pos = myvec::Vec3d(-2.79286, 5.51317, -0.131162);

	myvec::Vec3d local_vel = myvec::Vec3d();

	myvec::Vec3d f_drag = myvec::Vec3d();
	myvec::Vec3d f_drag_local = myvec::Vec3d();
	myvec::Vec3d drag_profile = myvec::Vec3d(0.22 * 5, 3.093 * 2, 0.074);

	float pitch_input = 0;
	float roll_input = 0;
	float max_input = 100;

	while (!quit)
	{
		std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(2);

		// handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				quit = true;
				break;

			case sf::Event::Resized:
				window_width = event.size.width;
				window_height = event.size.height;
				screen_center.x = window.getSize().x / 2; screen_center.y = window.getSize().y / 2;
				shader.setUniform("resolution", sf::Glsl::Vec2(window_width, window_height));
				break;

			case sf::Event::MouseMoved:
				mouse_pos.x = event.mouseMove.x;
				mouse_pos.y = event.mouseMove.y;
				break;

			case sf::Event::MouseWheelScrolled:
				move_speed += move_speed * (float)event.mouseWheelScroll.delta * 0.1;
				break;

			case sf::Event::MouseButtonPressed:

				scene.shader->setUniform("light_dir", sf::Glsl::Vec3(scene.CamGetForward().x, scene.CamGetForward().y, scene.CamGetForward().z));
				break;

			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Key::Escape)
				{
					mouse_is_locked = !mouse_is_locked;

					if (mouse_is_locked)
					{
						window.setMouseCursorVisible(false);
						sf::Mouse::setPosition(screen_center);

						pos = myvec::Vec3d(-2.81, 8, -0.141162);
						vel = myvec::Vec3d(0, 0, 0);
					}
					else
					{
						window.setMouseCursorVisible(true);
					}
				}
				break;
			}
		}
		/*
		local_vel = myvec::Vec3d(vel.dot(scene.CamGetRight()), vel.dot(scene.CamGetUp()), vel.dot(scene.CamGetForward()));

		f_drag_local = myvec::Vec3d(
			drag_profile.x * local_vel.x * -abs(local_vel.x),
			drag_profile.y * local_vel.y * -abs(local_vel.y),
			drag_profile.z * local_vel.z * -abs(local_vel.z)
		);


		f_drag = scene.CamGetRight() * f_drag_local.x + scene.CamGetUp() * f_drag_local.y + scene.CamGetForward() * f_drag_local.z;

		acc = f_drag + myvec::Vec3d(0, -0.000000098, 0);
		vel += acc;
		pos += vel;

		myvec::Vec3d turn_ang = myvec::Vec3d(0);

		scene.SetCamFocalLength(std::clamp(1000 - vel.length() * 6e5, (double)700, (double)1000));

		// input
		if (mouse_is_locked)
		{
			int W = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W);
			int S = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);
			int A = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
			int D = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);

			if (W - S == 0 && abs(pitch_input) > 0)
			{
				pitch_input -= pitch_input * 0.01;
			}
			if (abs(pitch_input) < max_input)
			{
				pitch_input += (W - S) * (abs(pitch_input) + 10) * 0.01;
			}

			if (A - D == 0 && abs(roll_input) > 0)
			{
				roll_input -= roll_input * 0.01;
			}
			else if (abs(roll_input) < max_input)
			{
				roll_input += (A - D) * (abs(roll_input) + 10) * 0.01;
			}

			turn_ang += myvec::Vec3d(
				pitch_input,
				0,
				roll_input
			) * 0.0001;
			
		}

			
		turn_ang += myvec::Vec3d(
			-local_vel.y * abs(local_vel.y) * 2e5,
			local_vel.x * abs(local_vel.x) * 2e5,
			0
		);
			



		scene.TurnCam(turn_ang);
		scene.SetCamPos(pos);
		*/
			
		//std::cout << (1000 - vel.length() * 6e5) << std::endl;

		if (mouse_is_locked)
		{

			sf::Vector2i mouse_delta = mouse_pos - screen_center;
			sf::Mouse::setPosition(screen_center, window);


			// free cam
			
			scene.TurnCam( myvec::Vec3d(
					mouse_delta.y,
					mouse_delta.x,
					sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q) - sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E)
				)
					* mouse_sens
			);

			scene.MoveCam(myvec::Vec3d(
					sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) - sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A),
					sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) - sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift),
					sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) - sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)
				)
				* move_speed
			);
			
			

		}
		

		std::this_thread::sleep_until(t1);
	}

	scene_dispatcher.join();
	window.close();

	return EXIT_SUCCESS;
}


/*
#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <SFML/Graphics.hpp>
#include "myvec.h"
#include "scene.h"

bool quit = false;

int window_width = 1920;
int window_height = 1080;

sf::Vector2i mouse_pos;
float mouse_sens = 0.003f;
bool mouse_is_locked = false;

float move_speed = 0.00009656064;

void UpdateScene(sf::RenderWindow* window, sf::Shader* shader, Scene* scene)
{
	window->setActive(true);

	// create screen rectangle
	sf::RectangleShape rect;
	rect.setSize(sf::Vector2f(window_width, window_height));
	rect.setPosition(0, 0);

	while (!quit)
	{
		std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(1);

		scene->UpdateCam();

		window->clear();
		window->draw(rect, shader);
		window->display();

		std::this_thread::sleep_until(t1);
	}
}

int main(int argc, char** argv)
{
	// check if gpu supports shaders
	if (!sf::Shader::isAvailable()) {
		std::cout << "GPU does not support shaders." << std::endl;
		return EXIT_FAILURE;
	}

	sf::Shader shader;

	// Load fragment shader
	if (!shader.loadFromFile("frag.glsl", sf::Shader::Fragment)) {
		std::cout << "Failed to compile fragment shader." << std::endl;
		return EXIT_FAILURE;
	}

	shader.setUniform("resolution", sf::Glsl::Vec2(window_width, window_height));

	// create the window
	sf::RenderWindow window(sf::VideoMode(window_width, window_height), "test");

	sf::Vector2i screen_center(window.getSize().x / 2, window.getSize().y / 2);
	//LockMouse(window);
	mouse_is_locked = true;
	window.setMouseCursorVisible(false);

	// create scene
	Scene scene = Scene(&shader);

	window.setActive(false); // yield window to render thread
	std::thread scene_dispatcher(UpdateScene, &window, &shader, &scene);

	myvec::Vec3d acc = myvec::Vec3d();
	myvec::Vec3d vel = myvec::Vec3d();
	myvec::Vec3d pos = myvec::Vec3d(-2.79286, 5.51317, -0.131162);

	myvec::Vec3d local_vel = myvec::Vec3d();

	myvec::Vec3d f_drag = myvec::Vec3d();
	myvec::Vec3d f_drag_local = myvec::Vec3d();
	myvec::Vec3d drag_profile = myvec::Vec3d(0.22 * 5, 3.093 * 2, 0.074);

	while (!quit)
	{
		std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(2);

		// handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				quit = true;
				break;

			case sf::Event::Resized:
				window_width = event.size.width;
				window_height = event.size.height;
				screen_center.x = window.getSize().x / 2; screen_center.y = window.getSize().y / 2;
				shader.setUniform("resolution", sf::Glsl::Vec2(window_width, window_height));
				break;

			case sf::Event::MouseMoved:
				mouse_pos.x = event.mouseMove.x;
				mouse_pos.y = event.mouseMove.y;
				break;

			case sf::Event::MouseWheelScrolled:
				move_speed += move_speed * (float)event.mouseWheelScroll.delta * 0.1;
				break;

			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Key::Escape)
				{
					mouse_is_locked = !mouse_is_locked;

					if (mouse_is_locked)
					{
						window.setMouseCursorVisible(false);
						sf::Mouse::setPosition(screen_center);

						pos = myvec::Vec3d(-2.81, 5.51317, -0.141162);
						vel = myvec::Vec3d(0, 0, 0);
					}
					else
					{
						window.setMouseCursorVisible(true);
					}
				}
				break;
			}
		}

		local_vel = myvec::Vec3d(vel.dot(scene.CamGetRight()), vel.dot(scene.CamGetUp()), vel.dot(scene.CamGetForward()));

		f_drag_local = myvec::Vec3d(
			drag_profile.x * local_vel.x * -abs(local_vel.x),
			drag_profile.y * local_vel.y * -abs(local_vel.y),
			drag_profile.z * local_vel.z * -abs(local_vel.z)
		);


		f_drag = scene.CamGetRight() * f_drag_local.x + scene.CamGetUp() * f_drag_local.y + scene.CamGetForward() * f_drag_local.z;
		//f_drag = vel.normalized() * vel.length2() *  - 1 * 0.074;

		acc = f_drag + myvec::Vec3d(0, -0.000000098, 0);
		vel += acc;
		// vel = myvec::Vec3d(0, -0.000178, 0);
		pos += vel;
		// 0.00115 terminal head first vel
		// 0.000178 terminal flat

		//std::cout << scene.CamGetPos().x << ", " << scene.CamGetPos().y << ", " << scene.CamGetPos().z << std::endl;

		// input
		if (mouse_is_locked)
		{

			sf::Vector2i mouse_delta = mouse_pos - screen_center;
			sf::Mouse::setPosition(screen_center, window);

			float turncap = 8;

			scene.TurnCam(myvec::Vec3d(
				std::clamp(-(float)mouse_delta.y, -turncap, turncap) - local_vel.y * abs(local_vel.y) * 10e7,
				sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) - sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) * 0.3 + local_vel.x * abs(local_vel.x) * 10e7,
				std::clamp(-(float)mouse_delta.x, -turncap, turncap)
			)
				* mouse_sens
			);

			scene.SetCamPos(pos);


			// free cam
			/*
			scene.TurnCam( myvec::Vec3d(
					mouse_delta.y,
					mouse_delta.x,
					sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q) - sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E)
				)
					* mouse_sens
			);

			scene.MoveCam(myvec::Vec3d(
					sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) - sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A),
					sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) - sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift),
					sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) - sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)
				)
				* move_speed
			);
			*/
/*
std::cout << vel.length() << std::endl;
//std::cout << scene.CamGetPos().x << ", " << scene.CamGetPos().y << ", " << scene.CamGetPos().z << std::endl
		}

		std::this_thread::sleep_until(t1);
	}

	scene_dispatcher.join();
	window.close();

	return EXIT_SUCCESS;
}
*/
