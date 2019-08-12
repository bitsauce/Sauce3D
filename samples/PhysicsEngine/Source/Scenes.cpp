#include "Scenes.h"

const uint32_t randomSeed = 453423120;

void SceneManager::initialize(const ExampleScene scene)
{
	clear();

	switch(scene)
	{
		case SCENE_EMPTY:
			break;
		case SCENE_ENCLOSURE:
			initializeEnclosureScene();
			break;
		case SCENE_BENCHMARK_CIRCLES:
			initializeEnclosureScene();
			initializeCirclesBenchmarkScene();
			break;
	}
}

void SceneManager::clear()
{
	for(Body *body : m_bodies)
	{
		delete body;
	}
	m_bodies.clear();
}

void SceneManager::initializeEnclosureScene()
{
	Window *window = Game::Get()->getWindow();
	const Vector2I size = window->getSize();

	// Create ground
	{
		Body *body = new Body();
		body->setPosition(Vector2F(size.x * 0.5f, size.y - 10.0f));
		body->setMass(0.0f);
		body->setInertia(0.0f);

		Box *box = new Box;
		box->setSize(Vector2F(size.x, 20.0f));
		body->addShape(box);

		m_bodies.push_back(body);
	}

	// Create right wall
	{
		Body *body = new Body();
		body->setPosition(Vector2F(size.x - 10.0f, size.y * 0.5f));
		body->setMass(0.0f);
		body->setInertia(0.0f);

		Box *box = new Box;
		box->setSize(Vector2F(20.0f, size.y));
		body->addShape(box);

		m_bodies.push_back(body);
	}

	// Create left wall
	{
		Body *body = new Body();
		body->setPosition(Vector2F(10.0f, size.y * 0.5f));
		body->setMass(0.0f);
		body->setInertia(0.0f);

		Box *box = new Box;
		box->setSize(Vector2F(20.0f, size.y));
		body->addShape(box);

		m_bodies.push_back(body);
	}

	// Create roof
	{
		Body *body = new Body();
		body->setPosition(Vector2F(size.x * 0.5f, 10.0f));
		body->setMass(0.0f);
		body->setInertia(0.0f);

		Box *box = new Box;
		box->setSize(Vector2F(size.x, 20.0f));
		body->addShape(box);

		m_bodies.push_back(body);
	}
}

void SceneManager::initializeCirclesBenchmarkScene()
{
	Window *window = Game::Get()->getWindow();
	const Vector2I size = window->getSize();

	const float radiusMin = 2.0f;
	const float radiusMax = 20.0f;

	Random rand(randomSeed);
	for(int i = 0; i < g_benchmarkSceneNumCircles; ++i)
	{
		const float radius = rand.nextDouble(radiusMin, radiusMax);
		const Vector2F spawnPoint = Vector2F(rand.nextDouble(20.0f+radius, size.x-20.0f-radius), rand.nextDouble(20.0f+radius, size.x-20.0f-radius));

		Body *body = new Body();
		body->setPosition(spawnPoint);

		Circle *circle = new Circle;
		circle->setRadius(radius);
		body->addShape(circle);

		m_bodies.push_back(body);
	}
}
